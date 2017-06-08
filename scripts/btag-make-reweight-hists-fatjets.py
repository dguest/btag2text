#!/usr/bin/env python3
"""
Make b-tagging reweighting histograms
"""
from ndhist.hist import Hist
from h5py import File
from argparse import ArgumentParser
import numpy as np
import copy

_default_output = 'reweight.h5'

def _get_args():
    d = "default: %(default)s"
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input')
    parser.add_argument('output', nargs='?', default=_default_output,
                        help=d)
    return parser.parse_args()

def run():
    args = _get_args()
    with File(args.input, 'r') as h5in:
        numerator = Hist(h5in['/hists/qcd/fatjet/pt'])
        denominator = Hist(h5in['/hists/signal/fatjet/pt'])
    rw_hist = copy.deepcopy(denominator)
    nonzero = rw_hist.hist != 0
    rw_hist.hist = numerator.hist[nonzero] / denominator.hist[nonzero]
    with File(args.output, 'w') as h5out:
        rw_hist.write(h5out, 'signal_weights')


if __name__ == '__main__':
    run()

