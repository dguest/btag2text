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
