#!/usr/bin/env python3

"""
Draw some diagnostics plots from an hdf5 file
"""

from argparse import ArgumentParser
from h5py import File
from ndhist.mpl import Canvas
import numpy as np

def get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input_file')
    parser.add_argument('-n', '--nevents', default=100000)
    parser.add_argument('-o', '--output-dir', default='plots')
    return parser.parse_args()

def run():
    args = get_args()
    with File(args.input_file, 'r') as h5file:
        jets_chunk = h5file['jets'][:args.nevents]
    draw_jets(jets_chunk, args.output_dir)

def draw_jets(jets_chunk, out_dir):
    signal_idx = (jets_chunk['LabDr_HadF'] == 5)
    bg_idx = (jets_chunk['LabDr_HadF'] == 0)
    signal = jets_chunk[signal_idx]
    bg = jets_chunk[bg_idx]
    for var in jets_chunk.dtype.names:
        with Canvas(f'{out_dir}/{var}.pdf') as can:
            def get_good(vals):
                good = ~np.isnan(vals[var])
                return vals[var][good]
            can.ax.hist(get_good(signal), color=(1, 0, 0, 0.5))
            can.ax.hist(get_good(bg), color=(0, 0, 1, 0.5))

if __name__ == '__main__':
    run()
