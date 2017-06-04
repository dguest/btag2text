#!/usr/bin/env python3
"""
Make mv2 ROC curves
"""
from ndhist.hist import Hist
from ndhist.hist import HistError
from ndhist.mpl import Canvas, draw1d, draw2d
from h5py import File
from argparse import ArgumentParser
import numpy as np

import os

red = (1, 0, 0, 0.8)
green = (0, 0.5, 0, 0.8)
blue = (0, 0, 1, 0.8)
_colors = {'qcd': blue,'signal': red}
_default_output = 'plots'

def _get_args():
    d = "default: %(default)s"
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input')
    parser.add_argument(
        '-o', '--output-dir', nargs='?', default=_default_output, help=d)
    parser.add_argument('-l', '--log', action='store_true')
    parser.add_argument('-e', '--ext', default='.pdf', help=d)
    return parser.parse_args()

# utility stuff
def _cumsum(ds):
    return np.asarray(ds)[::-1].cumsum(axis=0)

def _reg(arr):
    rej = np.zeros(arr.shape)
    valid = arr != 0
    rej[valid] = arr.max() / arr[valid]
    invalid = np.logical_not(valid)
    rej[invalid] = np.inf
    return rej

def _eff(arr):
    return arr / arr.max()

MIN_EFF = 0.5
def run():
    args = _get_args()
    with File(args.input, 'r') as h5in:
        def get_cumsum(sample):
            return _cumsum(h5in['hists'][sample]['fatjet']['mv2c10_min'])
        eff = _eff(get_cumsum('signal'))
        rej = _reg(get_cumsum('qcd'))

    if not os.path.isdir(args.output_dir):
        os.makedirs(args.output_dir)

    print('plotting')
    high_eff = eff > MIN_EFF
    with Canvas(os.path.join(args.output_dir, f'thing{args.ext}')) as can:
        can.ax.plot(eff[high_eff], rej[high_eff], label='min mv2')
        can.ax.set_yscale('log')
        can.ax.set_xlabel(r'$b$ efficiency')
        can.ax.set_ylabel(r'qcd rejection')
        can.ax.grid(True)
        can.ax.set_xlim(0.5, 1.0)
        can.ax.legend(framealpha=0)

if __name__ == '__main__':
    run()

