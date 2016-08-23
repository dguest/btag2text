#!/usr/bin/env python3
"""Add histograms"""
from ndhist.hadd import hadd
from argparse import ArgumentParser
import re, os
from collections import defaultdict

def _get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("in_files", nargs="+")
    out_format = parser.add_mutually_exclusive_group(required=True)
    out_format.add_argument('-o', "--out-file")
    out_format.add_argument('-d', '--out-dir')
    parser.add_argument('-v', '--verbose', action='store_true')
    return parser.parse_args()

def cluster(files):
    file_regex = re.compile('.*d([0-9]+)_i([0-9]+)_j([0-9]+).*')
    datasets = defaultdict(list)
    for path in files:
        dsid, idx, job = file_regex.match(path).groups()
        datasets[int(dsid)].append(path)
    return datasets

def run():
    args = _get_args()
    if args.out_file:
        hadd(args.out_file, args.in_files, verbose=args.verbose)
    elif args.out_dir:
        if not os.path.isdir(args.out_dir):
            os.mkdir(args.out_dir)

        def hadd_wrap(ds, files, verbose=args.verbose):
            out_file_name = '{}.h5'.format(ds)
            out_path = os.path.join(args.out_dir, out_file_name)
            hadd(out_path, files)
            if verbose:
                print('made {}'.format(out_file_name))

        for ds, files in cluster(args.in_files).items():
            hadd_wrap(ds, files)


if __name__ == '__main__':
    run()
