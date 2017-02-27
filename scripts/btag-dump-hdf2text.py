#!/usr/bin/env python3

"""Dump HDF5 file containing single b-tagging data to a text
file. Also supports dumping a subset of track / jet variables, which
you specify with the flags below.
"""
_help_dump='Dump list of variable names rather than the values'

import argparse
import h5py
import sys
from json import dumps
import numpy as np
from signal import signal, SIGPIPE, SIG_DFL

ALL = 'all'

def _get_args():
    opts = dict(nargs='*', default=ALL, metavar='var',
                help='with no arguments dumps nothing')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input_file')
    parser.add_argument('-j', '--jet-variables', **opts)
    parser.add_argument('-t', '--track-variables', **opts)
    parser.add_argument('-d', '--dump-variables', action='store_true',
                        help=_help_dump)
    return parser.parse_args()

def _get_tracks(tracks, tvars):
    unmasked_idx = np.flatnonzero(~tracks['mask'])
    out_tracks = []
    for idx in unmasked_idx:
        out_tracks.append([np.asscalar(tracks[idx][v]) for v in tvars])
    return out_tracks

def _run():
    args = _get_args()
    h5file = h5py.File(args.input_file,'r')
    jets = h5file['jets']

    # ignore broken pipes
    signal(SIGPIPE, SIG_DFL)

    n_entries = jets.shape[0]

    jvars = jets.dtype.names
    if 'tracks' in h5file:
        tracks = h5file['tracks']
        tvars = [v for v in tracks.dtype.names if v != 'mask']
    if args.dump_variables:
        sys.stdout.write('jets: {}\ntracks: {}\n'.format(
            ', '.join(jvars), ', '.join(tvars)))
        exit()

    if args.jet_variables != ALL:
        jvars = args.jet_variables

    if args.track_variables != ALL:
        tvars = args.track_variables

    for entry in range(n_entries):
        jet_vars = [np.asscalar(jets[entry][v]) for v in jvars]
        if tvars:
            track_vars = _get_tracks(tracks[entry,:], tvars)
            output = [jet_vars, track_vars]
        else:
            output = jet_vars
        sys.stdout.write(dumps(output) + '\n')


if __name__ == '__main__':
    _run()
