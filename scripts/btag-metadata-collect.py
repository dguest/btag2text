#!/usr/bin/env python3
"""Collect metadata from histogram files. Saves as JSON."""
from argparse import ArgumentParser
from h5py import File
import re
from collections import defaultdict
import json

def _get_args():
    d = '(default: %(default)s)'
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("hist_files", nargs='+')
    parser.add_argument('-o', '--output-file', default='meta.json', help=d)
    parser.add_argument('-v', '--verbose', action='store_true')
    return parser.parse_args()

def run():
    file_regex = re.compile('.*d([0-9]+)_i([0-9]+)_j([0-9]+).*')
    args = _get_args()
    datasets = defaultdict(list)
    for fname in args.hist_files:
        dsid, idx, job = file_regex.match(fname).groups()
        datasets[int(dsid)].append(fname)

    # base entry for metadata
    def base_meta():
        return {'n_events': 0}
    metadict = defaultdict(base_meta)
    for ds, files in datasets.items():
        if args.verbose:
            print('getting {}'.format(ds))
        for h5name in files:
            with File(h5name,'r') as h5file:
                entries = int(h5file.attrs['n_entries'])
                metadict[ds]['n_events'] += entries

    with open(args.output_file, 'w') as outfile:
        json_metadict = {str(k): v for k, v in metadict.items()}
        print(json_metadict)
        outfile.write(json.dumps(json_metadict, indent=2))

if __name__ == '__main__':
    run()
