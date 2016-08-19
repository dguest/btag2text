#!/usr/bin/env python3
"""
Merge hists from different samples
"""
from h5py import File
from argparse import ArgumentParser

import os

def hist_pair(string):
    path, label = string.split()
    if not os.path.isfile(path):
        raise argparse.ArgumentTypeError("can't find {}".format(path))
    if not label.isdecimal():
        raise argparse.ArgumentTypeError(
            "{} isn't a flavor label".format(label))
    return path, int(label)

def _get_args():
    d = "default: %(default)s"
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('inputs', nargs='+', type=hist_pair)
    parser.add_argument('-o', '--out-file', default='.pdf', help=d)
    return parser.parse_args()

def run():
    args = _get_args()
    with File(args.out_file, 'w') as h5out:
        out_hists = h5out.create_group('hists')
        for path, label in args.inputs:
            with File(path,'r') as h5in:
                dest_group_name = str(label)
                # print('req group')
                dest = out_hists.require_group(dest_group_name)
                # print('copy')
                # print('dest', list(dest.keys()))
                # print('src', list(h5in.keys()))
                h5in.copy('hists', dest)
                # print('copied')


if __name__ == '__main__':
    run()

