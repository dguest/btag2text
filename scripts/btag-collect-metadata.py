#!/usr/bin/env python3
"""Add metadata"""
from argparse import ArgumentParser
from h5py import File
import re
from collections import defaultdict, Counter

def _get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("hist_files", nargs='+')
    parser.add_argument('-o', "--out-file", required=True)
    parser.add_argument('-v', '--verbose', action='store_true')
    return parser.parse_args()

def run():
    file_regex = re.compile('.*d([0-9]+)_i([0-9]+)_j([0-9]+).*')
    args = _get_args()
    datasets = defaultdict(list)
    for fname in args.hist_files:
        dsid, idx, job = file_regex.match(fname).groups()
        datasets[int(dsid)].append(fname)

    counter = Counter()
    for ds, files in datasets.items():
        if args.verbose:
            print('getting {}'.format(ds))
        for h5name in files:
            with File(h5name,'r') as h5file:
                counter[ds] += h5file.attrs['n_entries']

    print(counter)

if __name__ == '__main__':
    run()
