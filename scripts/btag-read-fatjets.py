#!/usr/bin/env python3

"""
Example script to read in btagging data
"""

import json
from argparse import ArgumentParser
import sys
import numpy as np
from itertools import chain
# from matplotlib.backends.backend_pdf import FigureCanvas as Canvas
# from matplotlib.figure import Figure
# from ndhist.mpl import Canvas

def _get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('input_file', nargs='?')
    return parser.parse_args()

def run():
    args = _get_args()
    if args.input_file:
        line_iter = iter(open(args.in_file))
    else:
        line_iter = sys.stdin

    for batch in _batch_iter(_track_iter(line_iter), 5, 3, ['pt']):
        print('batch:')
        print(batch)

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
_cluster_keys = ['pt', 'eta', 'dphi', 'energy']
_cluster_types = np.dtype([(x,'f8') for x in _cluster_keys])
def _cluster_iter(line_iter):
    for line in line_iter:
        jet = Jet(line)
        cluster_array = np.fromiter(jet.clusters, dtype=_cluster_types)
        yield cluster_array

def _batch_iter(line_iter, n_objects, batchsize, features):
    """Group objects into batches for ML (or just plotting)

    `line_iter` should yield a structured np array (which can be
    selected using `features`
    """
    batch = np.zeros((batchsize, n_objects, len(features)))
    sample_n = 0
    for array in line_iter:
        for feature_n, feature in enumerate(features):
            batch[sample_n, :, feature_n] = array[feature][:n_objects]
        sample_n += 1
        if sample_n == batchsize:
            yield batch
            sample_n = 0
            batch.fill(0)
    yield batch[:sample_n, ...]

# track reader / iterator
_track_keys = [
    ["pt", "eta", "dphi"],
    ["d0", "d0sig", "z0", "z0sig"],
    ["chi2", "ndf", "nBLHits", "expectBLayerHit",
     "nInnHits", "nNextToInnHits", "nPixHits", "nSCTHits"]]
_track_types = np.dtype([(x, 'f8') for x in chain(*_track_keys)])
def _track_iter(line_iter):
    for line in line_iter:
        jet = Jet(line)
        track_array = np.fromiter(jet.tracks, dtype=_track_types)
        yield track_array


if __name__ == '__main__':
    run()
