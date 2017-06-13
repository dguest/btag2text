#!/usr/bin/env python3

"""
Draw some roc curves plots from an hdf5 file
"""

from argparse import ArgumentParser
from h5py import File
from ndhist.mpl import Canvas
import numpy as np

def get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input_file')
    parser.add_argument('-n', '--nevents', default=100000, type=int)
    parser.add_argument('-o', '--output-dir', default='plots')
    return parser.parse_args()

def run():
    args = get_args()
    with File(args.input_file, 'r') as h5file:
        jets_chunk = h5file['jets'][:args.nevents]
    mv2 = jets_chunk['mv2c10']
    signal_idx = (jets_chunk['LabDr_HadF'] == 5)
    bg_idx = (jets_chunk['LabDr_HadF'] == 0)
    mv2_bins = np.linspace(np.min(mv2), np.max(mv2), 10000)
    sig_hist, edges = np.histogram(mv2[signal_idx], bins=mv2_bins)
    bg_hist, edges = np.histogram(mv2[bg_idx], bins=mv2_bins)
    odir = args.output_dir
    left, right = edges[:-1], edges[1:]
    x_vals = np.array([left, right]).T.flatten()
    y_sig = np.array([sig_hist, sig_hist]).T.flatten()
    y_bg = np.array([bg_hist, bg_hist]).T.flatten()
    with Canvas(f'{odir}/dists.pdf') as can:
         can.ax.plot(x_vals, y_sig, color='red', label='bottom')
         can.ax.plot(x_vals, y_bg, color='blue', label='light')
         can.ax.legend()
         can.ax.set_yscale('log')

    sig_int = np.cumsum(sig_hist[::-1])
    bg_int = np.cumsum(bg_hist[::-1])
    valid = bg_int > 0
    with Canvas(f'{odir}/roc.pdf') as can:
        can.ax.plot(sig_int[valid] / sig_int[valid].max(),
                    bg_int[valid].max() / bg_int[valid])
        can.ax.set_yscale('log')

if __name__ == '__main__':
    run()
