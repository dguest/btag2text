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

_default_output = "roc2d"
_labels = {0: "light", 4: "charm", 5: "bottom"}
red = (1, 0, 0, 0.8)
green = (0, 0.5, 0, 0.8)
blue = (0, 0, 1, 0.8)
_colors = {0: blue, 4: green, 5: red}

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

def _ax_cuts(axis, n_bins):
    cuts = np.linspace(axis.lims[1], axis.lims[0], n_bins - 1)
    return np.concatenate((cuts, [np.inf]))

# main run func
U_WT = 1.0
C_WT = 1.0
MIN_EFF = 0.5
def run():
    args = _get_args()
    subdirs = {}
    with File(args.input, 'r') as h5in:
        hists = h5in['hists']
        for hist_type in hists:
            subdirs[hist_type] = _get_hists(hists[hist_type])

    signal = subdirs['raw']['5']['mv2']
    bg = subdirs['raw']['0']['mv2']
    bg.hist = subdirs['raw']['4']['mv2'].hist * C_WT + bg.hist * U_WT
    if not os.path.isdir(args.output_dir):
        os.makedirs(args.output_dir)
    with Canvas(os.path.join(args.output_dir, 'test.pdf')) as can:
        draw2d(can, signal, log=True)

    # calculate 2d roc
    eff = _eff(_cumsum(signal.hist))
    rej = _reg(_cumsum(bg.hist))
    dtype = [('eff', float), ('rej', float),
             ('mv2c100', float), ('mv2c00', float)]
    roc_array = np.zeros(eff.shape, dtype=dtype)
    roc_array['eff'] = eff
    roc_array['rej'] = rej
    # index mv2 cuts
    axes = signal.axes
    roc_array[axes[0].name] = _ax_cuts(axes[0], eff.shape[0])[:, None]
    roc_array[axes[1].name] = _ax_cuts(axes[1], eff.shape[1])[None, :]
    sort_roc = np.sort(roc_array.flatten(), order='eff')
    valid = np.isfinite(sort_roc['rej'])
    sort_roc = sort_roc[valid]

    # calculate 1d roc
    roc_tups = {}
    for tagger in ['mv2c10', 'mv2c20']:
        eff = _eff(subdirs['raw']['5'][tagger].hist[::-1].cumsum())
        bg = subdirs['raw']['0'][tagger].hist
        bg = subdirs['raw']['4'][tagger].hist * C_WT + bg * U_WT
        rej = _reg(bg[::-1].cumsum())
        roc_tups[tagger] = (eff, rej)

    def op(base):
        return os.path.join(args.output_dir, '{}{}'.format(base, args.ext))

    print('plotting')
    cum_max = np.maximum.accumulate(sort_roc['rej'][::-1])[::-1]
    high_index = (cum_max == sort_roc['rej'])
    next_point_lower = (cum_max[:-1] > cum_max[1:])
    high_index[~next_point_lower] = False
    high_eff = sort_roc['eff'] > MIN_EFF
    with Canvas(op('roc_raw')) as can:
        # can.ax.plot(sort_roc['eff'], sort_roc['rej'], label='all points')
        can.ax.plot(sort_roc['eff'][high_index & high_eff],
                    sort_roc['rej'][high_index & high_eff],
                    label='upper points')
        for tagger, (eff, rej) in roc_tups.items():
            high_eff = eff > MIN_EFF
            can.ax.plot(eff[high_eff], rej[high_eff], label=tagger)
        can.ax.set_yscale('log')
        can.ax.set_xlabel(r'$b$ efficiency')
        can.ax.set_ylabel(r'inclusive bg rejection')
        can.ax.grid(True)
        can.ax.set_xlim(0.5, 1.0)
        can.ax.legend(framealpha=0)

    print('plotting cut path')
    with Canvas(op('cut_scatter')) as can:
        x, y = axes
        can.ax.plot(sort_roc[x.name][high_index],
                    sort_roc[y.name][high_index],
                    marker=None, linestyle='-')
        can.ax.set_xlabel(x.name)
        can.ax.set_ylabel(y.name)

    print('plotting cut vs eff')
    with Canvas(op('cut_vs_eff')) as can:
        x, y = axes
        can.ax.plot(sort_roc['eff'][high_index],
                    sort_roc[x.name][high_index],
                    label=x.name)
        can.ax.plot(sort_roc['eff'][high_index],
                    sort_roc[y.name][high_index],
                    label=y.name)
        can.ax.set_xlabel(r'$b$-efficiency')
        can.ax.set_ylabel(r'cut value')
        can.ax.legend(framealpha=0)

if __name__ == '__main__':
    run()

