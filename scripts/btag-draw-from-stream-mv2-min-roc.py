#!/usr/bin/env python3

"""
Draw the minimum MV2 roc curve for an h->bb sample
"""

from argparse import ArgumentParser
from ndhist.mpl import Canvas
import numpy as np
import os, sys, json

def get_args():
    d = "default: %(default)s"
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('-o', '--output-dir', default='plots')
    parser.add_argument('-e', '--ext', default='.pdf', help=d)
    parser.add_argument('-v', '--verbose', action='store_true')
    return parser.parse_args()


MIN_EFF = 0.1
def run():
    args = get_args()
    if not sys.stdin.isatty():
        inputs = json.load(sys.stdin)
    else:
        sys.exit("input must be piped")

    eff = np.asarray(inputs['eff'])
    rej = np.asarray(inputs['rej'])

    if not os.path.isdir(args.output_dir):
        os.makedirs(args.output_dir)

    if args.verbose:
        sys.stderr.write('plotting\n')
    high_eff = eff >= MIN_EFF

    roc_out_path = os.path.join(args.output_dir, f'mv2-min-roc{args.ext}')
    with Canvas(roc_out_path) as can:
        can.ax.plot(eff[high_eff], rej[high_eff], label='min mv2')
        can.ax.set_yscale('log')
        can.ax.set_xlabel(r'$b$ efficiency')
        can.ax.set_ylabel(r'qcd rejection')
        can.ax.grid(True)
        can.ax.set_xlim(0.1, 1.0)
        can.ax.legend(framealpha=0)


if __name__ == '__main__':
    run()
