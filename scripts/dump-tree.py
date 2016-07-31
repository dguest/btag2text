#!/usr/bin/env python2

"""
dumps info on branches in a tree
"""

# Unshittify ROOT
from ROOT import PyConfig
PyConfig.IgnoreCommandLineOptions = True

from ROOT import TFile
from argparse import ArgumentParser
import re

_name_map = {
    'Int_t': 'int',
    'Double_t': 'double',
    'Float_t': 'float',
    'Bool_t': 'bool'
}

class Branch(object):
    def __init__(self, branch):
        self._class = True
        type_name = branch.GetClassName()
        if not type_name:
            self._class = False
            leafs = branch.GetListOfLeaves()
            assert len(leafs) == 1, "can't support multiple leafs in a branch"
            type_name = _name_map[leafs.At(0).GetTypeName()]
        self.type_name = type_name
        self.name = branch.GetName()
        self._std_regex = re.compile('(vector)')

    def declare_line(self):
        star = '*' if self._class else ''
        std_type = self._std_regex.sub(r'std::\1', self.type_name)
        return ''.join([std_type, star, ' ', self.name, ';'])

    def branch_line(self, chain_name, ptr=True):
        dref = '->' if ptr else '.'
        return '{c}{d}SetBranchAddress("{n}", &{n});'.format(
            c=chain_name, d=dref, n=self.name)

    def __str__(self):
        return '{} {}'.format(self.type_name,self.name)


def dumpall(d):
    """dump names of objects"""
    for key in d.GetListOfKeys():
        kname = key.GetName()
        print kname

def get_args():
    d = 'default: %(default)s'
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('file')
    parser.add_argument('-t', '--tree', default="bTag_AntiKt4EMTopoJets",
                        help=d);
    action = parser.add_mutually_exclusive_group()
    action.add_argument('-d', '--declare', action='store_true')
    action.add_argument('-s', '--set', action='store_true')
    return parser.parse_args()

if __name__ == '__main__':
    args = get_args()
    rfile = TFile(args.file)
    tree = rfile.Get(args.tree)
    if not tree:
        print "objects:"
        dumpall(rfile)
        exit("couldn't find tree " + args.tree)
    branches = tree.GetListOfBranches()
    for tbranch in branches:
        branch = Branch(tbranch)
        if args.declare:
            print branch.declare_line()
        elif args.set:
            print branch.branch_line('chain')
        else:
            print branch
