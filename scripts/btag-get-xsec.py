#!/usr/bin/env python3
"""
Get cross sections via pipe

"""

from argparse import ArgumentParser
import sys

def _get_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('xsec_file')
    if sys.stdin.isatty():
        parser.print_usage()
        exit('ERROR: need to pipe in dsids')
    return parser.parse_args()

def input_itr():
    for line in sys.stdin:
        yield from (int(x) for x in line.split())

class CrossSections:
    def __init__(self, file_name):
        self.datasets = {}
        with open(file_name) as infile:
            for rawline in infile:
                self._add_line(rawline)

    def _add_line(self, rawline):
        line = rawline.strip()
        if not line or line.startswith('#'):
            return
        _, dsid_s, xsec_s, filteff_s, *crap = line.split()
        self.datasets[int(dsid_s)] = {
            'xsec': float(xsec_s), 'filteff': float(filteff_s)}

    def get_weight(self, dsid):
        rec = self.datasets[dsid]
        return rec['xsec'] * rec['filteff']

def run():
    args = _get_args()
    xsecs = CrossSections(args.xsec_file)
    for dsid in input_itr():
        try:
            outline = '{}\n'.format(xsecs.get_weight(dsid))
        except KeyError as err:
            sys.stderr.write('ERROR: no xsec for {}'.format(dsid))
            exit(1)
        sys.stdout.write(outline)



if __name__ == '__main__':
    run()