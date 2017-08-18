#!/usr/bin/env python3

"""sort a list of files into signal and background"""

from argparse import ArgumentParser
from cross_sections import CrossSections, get_dsid
import re, os

XSEC="~/tagging/atlas/btag-data-mule/data/xsec.txt"

def get_args():
    d = dict(help='default: %(default)s')
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('file_list', nargs='+')
    parser.add_argument('-x', '--xsec-file', default=XSEC, **d)
    parser.add_argument('-b', '--background', default='background.txt', **d)
    parser.add_argument('-s', '--signal', default='signal.txt', **d)
    return parser.parse_args()

def run():
    args = get_args()
    files = args.file_list
    bg_file = open(args.background, 'w')
    sig_file = open(args.signal, 'w')

    xsecs = CrossSections(os.path.expanduser(args.xsec_file))

    for in_file in files:
        if xsecs.is_signal(get_dsid(in_file)):
            sig_file.write(in_file.strip() + '\n')
        else:
            bg_file.write(in_file.strip() + '\n')

if __name__ == '__main__':
    run()
