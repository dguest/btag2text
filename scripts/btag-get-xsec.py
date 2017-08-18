#!/usr/bin/env python3
"""
Get cross sections via pipe

"""

from argparse import ArgumentParser
import sys
from cross_sections import CrossSections

def _get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('xsec_file')
    parser.add_argument('-s', '--is-signal', action='store_true')
    if sys.stdin.isatty():
        parser.print_usage()
        exit('ERROR: need to pipe in dsids')
    return parser.parse_args()

def input_itr():
    for line in sys.stdin:
        yield from (int(x) for x in line.split())


def run():
    args = _get_args()
    xsecs = CrossSections(args.xsec_file)
    for dsid in input_itr():
        try:
            if args.is_signal:
                if xsecs.is_signal(dsid):
                    exit(0)
                else:
                    exit(1)
            else:
                outline = '{}\n'.format(xsecs.get_weight(dsid))
        except KeyError as err:
            exit(1)
        sys.stdout.write(outline)



if __name__ == '__main__':
    run()
