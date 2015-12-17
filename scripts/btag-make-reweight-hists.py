#!/usr/bin/env python3
"""
Make b-tagging reweighting histograms
"""
from ndhist.hist import Hist
from h5py import File
from argparse import ArgumentParser
import numpy as np

_default_output = 'reweight.h5'

def _get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input')
    parser.add_argument('output', nargs='?', default=_default_output)
    return parser.parse_args()

def run():
    args = _get_args()
    with File(args.input, 'r') as h5in:
        rw_hists = {n: Hist(ds) for n, ds in h5in.items()}
    total = sum(rw_hists.values())
    for name, hist in rw_hists.items():
        nonzero = hist.hist != 0
        hist.hist[nonzero] = total.hist[nonzero] / hist.hist[nonzero]
    with File(args.output, 'w') as h5out:
        for name, hist in rw_hists.items():
            hist.write(h5out, name)


if __name__ == '__main__':
    run()

