#!/usr/bin/env python3
"""
Draw b-tagging hists
"""
from ndhist.hist import Hist
from ndhist.hist import HistError
from ndhist.mpl import Canvas, draw1d
from h5py import File
from argparse import ArgumentParser
import numpy as np

import os

_default_output = "plots"
red = (1, 0, 0, 0.8)
green = (0, 0.5, 0, 0.8)
blue = (0, 0, 1, 0.8)
black = (0, 0, 0, 1.0)
_colors = {'qcd': blue, 4: green, 'signal': red, -1: black}
_force_log = {'pt', 'd0', 'd0sig', 'z0', 'z0sig', 'd0_signed',
              'JVT', 'mv1c00', 'mv1c10', 'mv1c20'}

def _get_args():
    d = "default: %(default)s"
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input')
    parser.add_argument(
        '-o', '--output-dir', nargs='?', default=_default_output, help=d)
    parser.add_argument('-l', '--log', action='store_true')
    parser.add_argument('-e', '--ext', default='.pdf', help=d)
    return parser.parse_args()

def _get_hists(ds):
    """Recussively convert hists to nested dictionary"""
    hists = {}
    for name, ds in ds.items():
        try:
            hists[name] = Hist(ds)
        except HistError:
            hists[name] = _get_hists(ds)
    return hists

def _draw_hists(hist_dict, output_dir, var='pt', log=False, ext='.pdf'):
    hists = []
    for name, hist in hist_dict.items():
        hist.color = _colors[name]
        hist.label = name
        hists.append(hist)
    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)
    name = '{}/{}{}'.format(output_dir, var if not log else var + '_log', ext)
    print("drawing {}".format(name))
    with Canvas(name) as can:
        draw1d(can, hists, log=log)
        can.ax.legend(framealpha=0)

def extended_variable_iter(proc_vars):
    for subjet, varhists in proc_vars.items():
        for var, hist in varhists.items():
            extended_var_name = '_'.join([subjet, var])
            yield extended_var_name, hist

def run():
    args = _get_args()
    with File(args.input, 'r') as h5in:
        hists = h5in['hists']
        processes = _get_hists(hists)

    variables = {}
    for proc_name, proc_vars in processes.items():
        for var, hist in extended_variable_iter(proc_vars):
            variables.setdefault(var, {})[proc_name] = hist

    for var, hists in variables.items():
        output_path = os.path.join(args.output_dir)
        _draw_hists(hists, output_path, var, log=args.log, ext=args.ext)
        if var in _force_log and not args.log:
            _draw_hists(hists, output_path, var, log=True, ext=args.ext)

if __name__ == '__main__':
    run()

