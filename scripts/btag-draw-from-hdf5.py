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
    parser.add_argument('-n', '--nevents', default=100000, type=int)
    parser.add_argument('-o', '--output-dir', default='plots')
    return parser.parse_args()

def run():
    args = get_args()
    with File(args.input_file, 'r') as h5file:
        jets_chunk = h5file['jets'][:args.nevents]
        # draw_jets(jets_chunk, args.output_dir)
        if 'tracks' in h5file:
            tracks_chunk = h5file['tracks'][:args.nevents]
            draw_tracks(tracks_chunk, jets_chunk, args.output_dir)

sig_color = (1, 0, 0, 0.5)
bg_color = (0, 0, 1, 0.5)

def add_nan_count(ax, count_sig, count_bg):
    ax.text(0.9,0.9, f'NaNs, sig: {count_sig}, bg: {count_bg}',
            va='top', ha='right',
            transform=ax.transAxes)

def draw_tracks(tracks_chunk, jets_chunk, out_dir):
    signal_idx = (jets_chunk['LabDr_HadF'] == 5)
    bg_idx = (jets_chunk['LabDr_HadF'] == 0)
    n_masked = np.count_nonzero(tracks_chunk['mask'], axis=1)
    n_tracks = tracks_chunk.shape[1] - n_masked
    count_bins = np.arange(-0.5, 20.5, 1)
    with Canvas(f'{out_dir}/n_tracks.pdf') as can:
        can.ax.hist(n_tracks[signal_idx], color=sig_color, bins=count_bins)
        can.ax.hist(n_tracks[bg_idx], color=bg_color, bins=count_bins)

    d0sig_bins = np.linspace(-50, 75, 40)
    leading_track_d0 = tracks_chunk['d0sig_ls'][:,0]
    valid_leading = ~tracks_chunk['mask'][:,0]
    with Canvas(f'{out_dir}/leading_track_d0sig.pdf') as can:
        can.ax.hist(leading_track_d0[valid_leading & signal_idx],
                    color=sig_color, label='bottom', bins=d0sig_bins)
        can.ax.hist(leading_track_d0[valid_leading & bg_idx],
                    color=bg_color, label='light', bins=d0sig_bins)
        can.ax.legend()
        can.ax.set_yscale('log')

    valid_sig_track = ~tracks_chunk['mask'] & signal_idx[:,None]
    valid_bg_track = ~tracks_chunk['mask'] & bg_idx[:,None]
    signal_d0sig = tracks_chunk['d0sig_ls'][valid_sig_track]
    bg_d0sig = tracks_chunk['d0sig_ls'][valid_bg_track]
    with Canvas(f'{out_dir}/track_d0sig.pdf') as can:
        can.ax.hist(leading_track_d0[valid_leading & signal_idx],
                    color=sig_color, label='bottom', bins=d0sig_bins)
        can.ax.hist(leading_track_d0[valid_leading & bg_idx],
                    color=bg_color, label='light', bins=d0sig_bins)
        can.ax.legend()
        can.ax.set_yscale('log')

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
            def count_nan(vals):
                return np.isnan(vals[var]).sum()
            can.ax.hist(get_good(signal), color=sig_color)
            can.ax.hist(get_good(bg), color=bg_color)
            can.ax.set_xlabel(var, x=0.98, ha='right', size=12)
            sig_nan, bg_nan = count_nan(signal), count_nan(bg)
            add_nan_count(can.ax, sig_nan, bg_nan)

if __name__ == '__main__':
    run()
