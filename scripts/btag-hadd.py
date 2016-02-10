#!/usr/bin/env python3
"""Add histograms"""
from ndhist.hadd import hadd
from argparse import ArgumentParser

def _get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("in_files", nargs="+")
    parser.add_argument('-o', "--out-file", required=True)
    return parser.parse_args()

def run():
    args = _get_args()
    hadd(args.out_file, args.in_files)

if __name__ == '__main__':
    run()
