#!/usr/bin/env python3
"""
Make b-tagging reweighting histograms
"""
from ndhist.hist import Hist
from ndhist.hist import HistError
from ndhist.mpl import Canvas, draw1d
from h5py import File
from argparse import ArgumentParser
import numpy as np

import os

_default_output = "plots"
_labels = {0: "light", 4: "charm", 5: "bottom"}
red = (1, 0, 0, 0.8)
green = (0, 0.5, 0, 0.8)
blue = (0, 0, 1, 0.8)
_colors = {0: blue, 4: green, 5: red}
_force_log = {'pt', 'd0', 'd0sig', 'z0', 'z0sig', 'd0_signed'}

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
    for ftl, name in _labels.items():
        hist = hist_dict[str(ftl)][var]
        hist.color = _colors[ftl]
        hist.label = name
        hists.append(hist)
    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)
    name = '{}/{}{}'.format(output_dir, var if not log else var + '_log', ext)
    print("drawing {}".format(name))
    with Canvas(name) as can:
        draw1d(can, hists, log=log)
        can.ax.legend(framealpha=0)

def run():
    args = _get_args()
    subdirs = {}
    with File(args.input, 'r') as h5in:
        hists = h5in['hists']
        for hist_type in hists:
            subdirs[hist_type] = _get_hists(hists[hist_type])

    for hist_type, hists in subdirs.items():
        for hname in hists['0']:
            output_path = os.path.join(args.output_dir, hist_type)
            _draw_hists(hists, output_path, hname, log=args.log, ext=args.ext)
            if hname in _force_log and not args.log:
                _draw_hists(hists, output_path, hname, log=True, ext=args.ext)

if __name__ == '__main__':
    run()

