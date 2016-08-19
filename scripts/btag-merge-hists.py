#!/usr/bin/env python3
"""
Merge hists from different samples

Inputs should be grouped by 'file_path,label'
"""
from h5py import File
from argparse import ArgumentParser, ArgumentTypeError
# from ndhist.hist import Hist, is_h5_hist
from ndhist.hadd import add_tree, build_h5_tree

import os

def hist_pair(string):
    pars = string.split(',')
    if len(pars) != 2:
        raise ArgumentTypeError("can't split '{}'".format(string))
    path, label = pars
    if not os.path.isfile(path):
        raise ArgumentTypeError("can't find {}".format(path))
    return path, label

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
            tree = {}
            with File(path,'r') as h5in:
                add_tree(tree, h5in['hists'])
            out_group = out_hists.create_group(label)
            build_h5_tree(out_group, tree)

if __name__ == '__main__':
    run()

