#!/usr/bin/env python3
"""Check to make sure the input file is valid json

Reads from stdin if no file is given
"""

import json
from argparse import ArgumentParser
import sys

def run():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('in_file', nargs='?')
    args = parser.parse_args()

    if args.in_file:
        with open(args.in_file) as ifile:
            string = next(ifile)
    else:
        string = next(sys.stdin)

    try:
        jdict = json.loads(string)
    except Exception as err:
        print('everthing is broken! {}'.format(str(err)))
        exit(1)
    if jdict:
        print('good!')

if __name__ == '__main__':
    run()

