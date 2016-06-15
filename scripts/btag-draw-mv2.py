#!/usr/bin/env python3
"""
Make b-tagging reweighting histograms
"""
from ndhist.hist import Hist
from ndhist.hist import HistError
from ndhist.mpl import Canvas, draw1d, draw2d
from h5py import File
from argparse import ArgumentParser
import numpy as np

import os

_default_output = "roc2d"
_labels = {0: "light", 4: "charm", 5: "bottom"}
red = (1, 0, 0, 0.8)
green = (0, 0.5, 0, 0.8)
blue = (0, 0, 1, 0.8)
_colors = {0: blue, 4: green, 5: red}
_force_log = {'pt', 'd0', 'd0sig', 'z0', 'z0sig', 'd0_signed',
              'JVT', 'mv1c00', 'mv1c10', 'mv1c20'}

def _get_args():
    d = "default: %(default)s"
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input')
    parser.add_argument(
        '-o', '--output-dir', nargs='?', default=_default_output, help=d)
    parser.add_argument('-l', '--log', action='store_true')
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

# utility stuff
def _cumsum(arr):
    return arr[::-1,::-1].cumsum(axis=0).cumsum(axis=1)

def _reg(arr):
    rej = np.zeros(arr.shape)
    valid = arr != 0
    rej[valid] = arr.max() / arr[valid]
    invalid = np.logical_not(valid)
    rej[invalid] = np.inf
    return rej

def _eff(arr):
    return arr / arr.max()

# main run func
def run():
    args = _get_args()
    subdirs = {}
    with File(args.input, 'r') as h5in:
        hists = h5in['hists']
        for hist_type in hists:
            subdirs[hist_type] = _get_hists(hists[hist_type])

    signal = subdirs['raw']['5']['mv2']
    bg = subdirs['raw']['0']['mv2']
    bg.hist += subdirs['raw']['4']['mv2'].hist * 10
    if not os.path.isdir(args.output_dir):
        os.makedirs(args.output_dir)
    with Canvas(os.path.join(args.output_dir, 'test.pdf')) as can:
        draw2d(can, signal, log=True)

    eff = _eff(_cumsum(signal.hist))
    rej = _reg(_cumsum(bg.hist))
    dtype = [('eff', float), ('rej', float), ('binx', int), ('biny', int)]
    roc_array = np.zeros(eff.shape, dtype=dtype)
    roc_array['eff'] = eff
    roc_array['rej'] = rej
    # TODO: fix (or check) this indexing
    roc_array['binx'] = np.arange(0, eff.shape[0])[:, None]
    roc_array['biny'] = np.arange(0, eff.shape[1])[None, :]
    sort_roc = np.sort(roc_array.flatten(), order='eff')

    valid = np.isfinite(sort_roc['rej'])
    sort_roc = sort_roc[valid]

    print('plotting')
    cum_max = np.maximum.accumulate(sort_roc['rej'][::-1])[::-1]
    high_index = cum_max == sort_roc['rej']
    with Canvas(os.path.join(args.output_dir, 'roc_raw.pdf')) as can:
        can.ax.plot(sort_roc['eff'], sort_roc['rej'], label='all points')
        can.ax.plot(sort_roc['eff'][high_index],
                    sort_roc['rej'][high_index], label='upper points')
        can.ax.set_yscale('log')
        can.ax.set_xlabel(r'$b$ efficiency')
        can.ax.set_ylabel(r'inclusive bg rejection')
        can.ax.legend()

    print('plotting cut path')
    with Canvas(os.path.join(args.output_dir, 'cut_scatter.pdf')) as can:
        can.ax.plot(sort_roc['binx'][high_index], sort_roc['biny'][high_index])

if __name__ == '__main__':
    run()

