#!/usr/bin/env python3
"""
Make b-tagging reweighting histograms
"""
from ndhist.hist import Hist
from ndhist.hist import HistError
from ndhist.mpl import Canvas
from ndhist.colorplot import draw_rgb
from h5py import File
from argparse import ArgumentParser
import numpy as np

import os

_default_output = "plots"

def _get_args():
    d = "default: %(default)s"
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input')
    parser.add_argument(
        '-o', '--output-dir', nargs='?', default=_default_output, help=d)
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

def run():
    args = _get_args()
    subdirs = {}
    with File(args.input, 'r') as h5in:
        hists = _get_hists(h5in['image'])
        for process, var_group in hists.items():
            green = var_group['em_energy']
            red = var_group['had_energy']
            blue = var_group['tracks']
            leg = {'red': 'had', 'green': 'em', 'blue': 'tracks'}
            hist_name = '{}_color{}'.format(process, args.ext)
            out_path = os.path.join(args.output_dir, hist_name)
            draw_rgb(red.hist, green.hist, blue.hist, red.axes,
                     out_path, leg)

if __name__ == '__main__':
    run()

