#!/usr/bin/env python3
"""Add metadata to xsec file"""
from argparse import ArgumentParser
import sys
import json

def _get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('-m', '--meta-file', required=True)
    parser.add_argument('-x', '--xsec-file', required=True)
    parser.add_argument('-v', '--verbose', action='store_true')
    return parser.parse_args()

def write_metadata(meta_dict, xsecfile, output=sys.stdout):
    for dirty_line in xsecfile:
        line, part, comment = dirty_line.strip().partition('#')
        if line:
            front, n_events = line.rsplit(maxsplit=1)
            dsid = front.split()[1]
            if dsid in meta_dict:
                sum_weights = meta_dict[dsid]['sum_event_weights']
            else:
                sum_weights = '?'
            line = '{} {:>9}'.format(front, sum_weights)
        output.write(line + part + comment + '\n')

def run():
    args = _get_args()
    with open(args.meta_file) as metafile:
        meta_dict = json.load(metafile)
    with open(args.xsec_file) as xsecfile:
        write_metadata(meta_dict, xsecfile)

if __name__ == '__main__':
    run()
