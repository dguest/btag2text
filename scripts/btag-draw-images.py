#!/usr/bin/env python3
"""
Make b-tagging reweighting histograms
"""
from ndhist.hist import Hist
from ndhist.hist import HistError
from ndhist.mpl import Canvas, draw2d
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

def _draw_hists(hist, hist_name, output_dir, ext='.pdf'):
    name = '{}/{}{}'.format(output_dir, 'image', ext)
    print("drawing {}".format(name))
    with Canvas(name) as can:
        draw2d(can, hist)

def run():
    args = _get_args()
    subdirs = {}
    with File(args.input, 'r') as h5in:
        hists = _get_hists(h5in['images/raw'])
        for hist_name, hist in hists.items():
            _draw_hists(hist, hist_name, args.output_dir, ext=args.ext)

if __name__ == '__main__':
    run()

