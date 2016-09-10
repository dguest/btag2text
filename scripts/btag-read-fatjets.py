#!/usr/bin/env python3

"""
Example script to read in btagging data
"""

import json
from argparse import ArgumentParser
import sys
import numpy as np
from itertools import chain
import os


BIG_BATCH=1000*1000

def _get_args():
    def const(help_string, const):
        hlp = '{} ({} with no args)'.format(help_string, const)
        return dict(nargs='?', help=hlp, const=const)
    def defopt(help_string, default_val):
        hlp = '{} (default "{}")'.format(help_string, default_val)
        return dict(help=hlp, default=default_val)
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input_file', nargs='?')
    parser.add_argument('-b', '--batchsize',
                        type=int, **defopt('batch sizes', BIG_BATCH))
    outputs = parser.add_mutually_exclusive_group(required=True)
    outputs.add_argument('-p', '--plots', **const('plots directory', 'plots'))
    outputs.add_argument('-f', '--hdf', **const('hdf output', 'jets.h5'))

    parser.add_argument('-e', '--ext', help='filename extension')

    parser.add_argument('-c', '--constituents', choices=_constituents.keys(),
                        default='clusters')
    return parser.parse_args()

def run():
    args = _get_args()
    if args.input_file:
        line_iter = iter(open(args.input_file))
    else:
        line_iter = sys.stdin

    # the producer should read in data and provide as an ndarray
    producer = _constituents[args.constituents](
        line_iter, max_length=40, batchsize=args.batchsize)

    if args.plots:
        fill_hists(producer, args.plots, args.ext)
    elif args.hdf:
        fill_hdf(producer, args.hdf)

# __________________________________________________________________________
# hdf5 export

def fill_hdf(producer, dest_file):
    from h5py import File
    obj_iter = iter(producer)
    first = next(obj_iter)
    shape = first.shape
    block_size = shape[0]
    with File(dest_file,'w') as h5file:
        ds = h5file.create_dataset(
            producer.name,
            shape=shape,
            chunks=shape,
            maxshape=( (None,) + shape[1:]),
            dtype=first.dtype,
            data=first)
        block_end = block_size
        for batch in obj_iter:
            block_size = batch.shape[0]
            new_end = block_end + block_size
            ds.resize(new_end, axis=0)
            ds[block_end:new_end, ...] = batch
            block_end = new_end

# _________________________________________________________________________
# plotting

_binning = {
    'energy': (101, 0, 20),
    'pt': (101, 0, 20),
    'eta': (101, -1, 1),
    'dphi': (101, -1, 1),
    'mask': (3, -0.5, 1.5),
}
def fill_hists(producer, dest_dir, ext):
    hists = {}
    for batch in producer:
        valid_mask = ~batch['mask']
        all_valid = np.ones(batch.shape, dtype=bool)
        for var in batch.dtype.names:
            valid = all_valid if var == 'mask' else valid_mask
            if var not in hists:
                wt = batch['weight'][valid]
                if var in _binning:
                    nbin, low, high = _binning[var]
                    bins = np.linspace(low, high, nbin)
                elif np.all(wt == 1.0):
                    bins = 'fd'
                    wt = None
                else:
                    low, high = batch[var].min(), batch[var].max()
                    bins = np.linspace(low, high, 100)

                vals = batch[var][valid]
                hist, bins = np.histogram(vals, weights=wt, bins=bins)
                hists[var] = {'h':np.array(hist, dtype=float), 'bins':bins}

            else:
                bins = hists[var]['bins']
                wt = batch['weight'][valid]
                vals = batch[var][valid]
                add_hist = np.histogram(vals, weights=wt, bins=bins)[0]
                hists[var]['h'] += add_hist

    for name, hist in hists.items():
        path = os.path.join(dest_dir, name + (ext or '.pdf'))
        print('plotting {}'.format(path))
        with Canvas(path) as can:
            draw_opts = dict(drawstyle='steps-post')
            can.ax.plot(*getxy(hist), **draw_opts)
            can.ax.set_xlabel(name)

def getxy(hist):
    """
    Return values for `plot` by clipping overflow bins and duplicating
    the final y-value.
    Assume we'll use `drawstyle='steps-post'`
    """
    y_vals = hist['h']
    x_vals = hist['bins']
    return x_vals, np.r_[y_vals, y_vals[-1]]


class Canvas:
    default_name = 'test.pdf'
    def __init__(self, out_path=None, figsize=(5.0,5.0*3/4), ext=None):
        # lazy import
        from matplotlib.backends.backend_agg import FigureCanvasAgg
        from matplotlib.figure import Figure

        self.fig = Figure(figsize)
        self.canvas = FigureCanvasAgg(self.fig)
        self.ax = self.fig.add_subplot(1,1,1)
        self.out_path = out_path
        self.ext = ext

    def save(self, out_path=None, ext=None):
        output = out_path or self.out_path
        assert output, "an output file name is required"
        out_dir, out_file = os.path.split(output)
        if ext:
            out_file = '{}.{}'.format(out_file, ext.lstrip('.'))
        if out_dir and not os.path.isdir(out_dir):
            os.makedirs(out_dir)
        self.canvas.print_figure(output, bbox_inches='tight')

    def __enter__(self):
        if not self.out_path:
            self.out_path = self.default_name
        return self
    def __exit__(self, extype, exval, extb):
        if extype:
            return None
        self.save(self.out_path, ext=self.ext)
        return True


# _________________________________________________________________________
# data handlers

class Jet:
    """class to load in and organize jet information"""
    def __init__(self, json_line):
        pt, eta, wt, trk, clusters, moments, subjets = json.loads(json_line)
        self.pt, self.eta = [pt, eta]
        self.wt = wt
        self.tracks = (tuple(chain(*tk)) for tk in trk)
        self.clusters = (tuple(cl) for cl in clusters)

# Iterators over colletions
# TODO: make keys read (or confirm) from the C++ `labels` output

# cluster reader / iterator
class Clusters:
    name = 'clusters'
    def __init__(self, in_file, max_length, batchsize):
        self._line_iter = in_file
        self._n_obj = max_length
        self._batchsize = batchsize
        self._keys = ['pt', 'eta', 'dphi', 'energy']
        self._types = np.dtype([(x,float) for x in self._keys])
    def _iter(self, line_iter):
        for line in line_iter:
            jet = Jet(line)
            cluster_array = np.fromiter(jet.clusters, dtype=self._types)
            yield cluster_array, jet.wt
    def __iter__(self):
        return _batch_iter(self._iter(self._line_iter),
                           self._n_obj,
                           self._batchsize,
                           self._types)

class Tracks:
    name = 'tracks'
    def __init__(self, in_file, max_length, batchsize=BIG_BATCH):
        self._line_iter = in_file
        self._n_obj = max_length
        self._batchsize = batchsize
        self._keys = [
            ["pt", "eta", "dphi"],
            ["d0", "d0sig", "z0", "z0sig"],
            ["chi2", "ndf", "nBLHits", "expectBLayerHit",
             "nInnHits", "nNextToInnHits", "nPixHits", "nSCTHits"]]
        self._types = np.dtype([(x,float) for x in chain(*self._keys)])
    def _iter(self, line_iter):
        for line in line_iter:
            jet = Jet(line)
            track_array = np.fromiter(jet.tracks, dtype=self._types)
            yield track_array, jet.wt
    def __iter__(self):
        return _batch_iter(self._iter(self._line_iter),
                           self._n_obj,
                           self._batchsize,
                           self._types)


def _batch_iter(line_iter, n_objects, batchsize, dtype):
    """Group objects into batches for ML (or just plotting)

    - `line_iter` should yield a structured np array
    - `n_objects` is the number of objects per jet (larger values are
      truncated, smaller values should be masked off)

    """
    new_dt = np.dtype(dtype.descr + [('mask',bool), ('weight', float)])
    batch = np.zeros((batchsize, n_objects), dtype=new_dt)
    sample_n = 0
    for array, wt in line_iter:

        n_missing = n_objects - array.size

        array.resize(n_objects, refcheck=False)
        batch[sample_n, :] = array
        batch['weight'][sample_n, :] = wt
        if n_missing > 0:
            batch['mask'][sample_n, -n_missing:] = True

        sample_n += 1
        if sample_n == batchsize:
            yield batch
            sample_n = 0
            batch.fill(0)
    yield batch[:sample_n, ...]

# to look up the builder by name we have this dictionary
_constituents = {'clusters':Clusters, 'tracks':Tracks}

if __name__ == '__main__':
    run()
