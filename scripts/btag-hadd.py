#!/usr/bin/env python3
"""
Add histograms. Inputs should be a list of HDF5 files
"""
_help_force = 'ignore missing files'
from ndhist.hadd import hadd
from argparse import ArgumentParser
import re, os, sys
from collections import defaultdict

def _get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("in_files", nargs="+")
    out_format = parser.add_mutually_exclusive_group(required=True)
    out_format.add_argument('-o', "--out-file")
    out_format.add_argument('-d', '--out-dir')
    parser.add_argument('-v', '--verbose', action='store_true')
    parser.add_argument(
        '-f', '--force', action='store_true', help=_help_force)
    return parser.parse_args()

def only_real_files(files):
    for the_file in files:
        if os.path.isfile(the_file):
            yield the_file
        else:
            sys.stdout.write('WARNING: {} is missing\n'.format(the_file))

def cluster(files):
    file_regex = re.compile('.*d([0-9]+)_i([0-9]+)_j([0-9]+).*')
    datasets = defaultdict(list)
    for path in files:
        dsid, idx, job = file_regex.match(path).groups()
        datasets[int(dsid)].append(path)
    return datasets

def run():
    args = _get_args()
    if args.force:
        in_files = only_real_files(args.in_files)
    else:
        in_files = args.in_files
    if args.out_file:
        hadd(args.out_file, in_files, verbose=args.verbose)
    elif args.out_dir:
        if not os.path.isdir(args.out_dir):
            os.mkdir(args.out_dir)

        def hadd_wrap(ds, files, verbose=args.verbose):
            out_file_name = '{}.h5'.format(ds)
            out_path = os.path.join(args.out_dir, out_file_name)
            hadd(out_path, files)
            if verbose:
                print('made {}'.format(out_file_name))

        for ds, files in cluster(in_files).items():
            hadd_wrap(ds, files)


if __name__ == '__main__':
    run()
