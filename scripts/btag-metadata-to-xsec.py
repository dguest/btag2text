#!/usr/bin/env python3
"""Add metadata to xsec file"""
from argparse import ArgumentParser

def _get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('-m', '--meta-file', required=True)
    parser.add_argument('-x', '--xsec-file', required=True)
    parser.add_argument('-v', '--verbose', action='store_true')
    return parser.parse_args()

def write_metadata(metafile, xsecfile):
    for dirty_line in xsecfile:
        line, part, comment = dirty_line.strip().partition('#')
        if line:
            print(line)
            front, n_events = line.rsplit(maxsplit=1)
            dsid = int(front.split()[1])
            line = '{} {}'.format(front, meta_dict.get(dsid, '?'))
        output_file.write(line + part + comment + '\n')

def run():
    args = _get_args()
    with open(args.meta_file) as metafile:
        with open(args.xsec_file, 'w') as xsecfile:
            write_metadata(metafile, xsecfile)

if __name__ == '__main__':
    run()
