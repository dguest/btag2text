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
_colors = {0: "blue", 4: "green", 5: "red"}

def _get_args():
    d = "default: %(default)s"
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input')
    parser.add_argument('output_dir', nargs='?', default=_default_output,
                        help=d)
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



def _draw_hists(hist_dict, output_dir, var='pt', prefix=''):
    hists = []
    for ftl, name in _labels.items():
        hist = hist_dict[str(ftl)][var]
        hist.color = _colors[ftl]
        hist.name = name
        hists.append(hist)
    if not os.path.isdir(output_dir):
        os.mkdir(output_dir)
    name = '{}/{}{}.pdf'.format(
        output_dir, prefix + "_" if prefix else '', var)
    with Canvas(name) as can:
        draw1d(can, hists)

def run():
    args = _get_args()
    with File(args.input, 'r') as h5in:
        raw_hists = _get_hists(h5in['raw'])
        reweighted = _get_hists(h5in['reweighted'])

    _draw_hists(raw_hists, args.output_dir, 'pt', prefix='raw')
    _draw_hists(reweighted, args.output_dir, 'pt', prefix='reweighted')
    _draw_hists(reweighted, args.output_dir, 'eta')

if __name__ == '__main__':
    run()

