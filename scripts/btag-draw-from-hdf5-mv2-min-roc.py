#!/usr/bin/env python3

"""
Draw the minimum MV2 roc curve for an h->bb sample
"""

from argparse import ArgumentParser
from h5py import File
from ndhist.mpl import Canvas
import numpy as np
import os

def get_args():
    d = "default: %(default)s"
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('-s', '--signal', nargs='+')
    parser.add_argument('-b', '--background', nargs='+')
    parser.add_argument('-n', '--batch-size', default=100000, type=int)
    parser.add_argument('-o', '--output-dir', default='plots')
    parser.add_argument('-e', '--ext', default='.pdf', help=d)
    parser.add_argument('-v', '--verbose', action='store_true')
    return parser.parse_args()

def get_dist(file_list, batch_size, nbins=10000, verbose=False):
    dist = np.zeros(nbins)
    edges = np.linspace(-1, 1, nbins+1)
    for fname in file_list:
        with File(fname, 'r') as h5file:
            n_entries = h5file['subjet1'].shape[0]
            for start_idx in range(0, n_entries, batch_size):
                end_idx = start_idx + batch_size
                if verbose:
                    print(f'{fname}, slice {start_idx}--{end_idx}')
                sj1 = h5file['subjet1'][start_idx:end_idx]
                sj2 = h5file['subjet2'][start_idx:end_idx]
                mv2c10_min = np.minimum(sj1['mv2c10'], sj2['mv2c10'])
                hist, _ = np.histogram(mv2c10_min, bins=edges)
                dist += hist
    return dist

MIN_EFF = 0.1
def run():
    args = get_args()
    opts = dict(batch_size = args.batch_size, verbose=args.verbose)
    signal = get_dist(args.signal, **opts)
    background = get_dist(args.background, **opts)
    eff = _eff(_cumsum(signal))
    rej = _rej(_cumsum(background))

    if not os.path.isdir(args.output_dir):
        os.makedirs(args.output_dir)

    print('plotting')
    high_eff = eff > MIN_EFF

    roc_out_path = os.path.join(args.output_dir, f'mv2-min-roc{args.ext}')
    with Canvas(roc_out_path) as can:
        can.ax.plot(eff[high_eff], rej[high_eff], label='min mv2')
        can.ax.set_yscale('log')
        can.ax.set_xlabel(r'$b$ efficiency')
        can.ax.set_ylabel(r'qcd rejection')
        can.ax.grid(True)
        can.ax.set_xlim(0.1, 1.0)
        can.ax.legend(framealpha=0)

# utility stuff
def _cumsum(ds):
    return np.asarray(ds)[::-1].cumsum(axis=0)

def _rej(arr):
    rej = np.zeros(arr.shape)
    valid = arr != 0
    rej[valid] = arr.max() / arr[valid]
    invalid = np.logical_not(valid)
    rej[invalid] = np.inf
    return rej

def _eff(arr):
    return arr / arr.max()


if __name__ == '__main__':
    run()
