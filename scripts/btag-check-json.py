#!/usr/bin/env python3
"""Check to make sure the input file is valid json

Reads from stdin if no file is given
"""

import json
from argparse import ArgumentParser
import sys
from collections import Counter

def _checkline(string):
    try:
        jdict = json.loads(string)
    except Exception as err:
        return False
    if jdict:
        return True

def run():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('in_file', nargs='?')
    args = parser.parse_args()

    if args.in_file:
        line_itr = ((l for l in open(args.in_file)))
    else:
        line_itr = sys.stdin

    lines = Counter()
    for line in line_itr:
        if _checkline(line) is True:
            lines['good'] += 1
        else:
            sys.stderr.write("BAD: {}".format(line))
            lines['bad'] += 1

    if lines['bad']:
        print('FAIL! {} good, {} bad'.format(lines['good'], lines['bad']))
    else:
        print('{} good lines!'.format(lines['good']))

if __name__ == '__main__':
    run()

