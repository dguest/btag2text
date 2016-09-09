#!/usr/bin/env python3

"""
Example script to read in btagging data
"""

import json
from argparse import ArgumentParser
import sys
import numpy as np
from itertools import chain
from h5py import File
# from matplotlib.backends.backend_pdf import FigureCanvas as Canvas
# from matplotlib.figure import Figure

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
    return parser.parse_args()

def run():
    args = _get_args()
    if args.input_file:
        line_iter = iter(open(args.input_file))
    else:
        line_iter = sys.stdin

    # the producer should read in data and provide as an ndarray
    producer = Clusters(line_iter, max_length=40, batchsize=args.batchsize)

    if args.plots:
        raise NotImplementedError("do this dude")
    elif args.hdf:
        fill_hdf(producer, args.hdf)

def fill_hdf(producer, dest_file):
    obj_iter = iter(producer)
    first = next(obj_iter)
    shape = first.shape
    block_size = shape[0]
    with File(dest_file,'w') as h5file:
        ds = h5file.create_dataset(
            'clusters',
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
    def __init__(self, in_file, max_length, batchsize):
        self._line_iter = in_file
        self._n_obj = max_length
        self._batchsize = batchsize
        self._keys = ['pt', 'eta', 'dphi', 'energy']
        self._types = np.dtype([(x,'f8') for x in self._keys])
    def _iter(self, line_iter):
        for line in line_iter:
            jet = Jet(line)
            cluster_array = np.fromiter(jet.clusters, dtype=self._types)
            yield cluster_array
    def __iter__(self):
        return _batch_iter(self._iter(self._line_iter),
                           self._n_obj,
                           self._batchsize,
                           self._types)

class Tracks:
    def __init__(self, in_file, max_length, batchsize=BIG_BATCH):
        self._line_iter = in_file
        self._n_obj = max_length
        self._batchsize = batchsize
        self._keys = [
            ["pt", "eta", "dphi"],
            ["d0", "d0sig", "z0", "z0sig"],
            ["chi2", "ndf", "nBLHits", "expectBLayerHit",
             "nInnHits", "nNextToInnHits", "nPixHits", "nSCTHits"]]
        self._types = np.dtype([(x, 'f8') for x in chain(*self._keys)])
    def _iter(self, line_iter):
        for line in line_iter:
            jet = Jet(line)
            track_array = np.fromiter(jet.tracks, dtype=self._types)
            yield track_array
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
    new_dt = np.dtype(dtype.descr + [('mask',bool)])
    batch = np.zeros((batchsize, n_objects), dtype=new_dt)
    sample_n = 0
    for array in line_iter:

        n_missing = n_objects - array.size

        array.resize(n_objects, refcheck=False)
        batch[sample_n, :] = array
        if n_missing > 0:
            batch['mask'][sample_n, -n_missing:] = True

        sample_n += 1
        if sample_n == batchsize:
            yield batch
            sample_n = 0
            batch.fill(0)
    yield batch[:sample_n, ...]


if __name__ == '__main__':
    run()
