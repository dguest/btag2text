#!/usr/bin/env python2

from ROOT import TFile
from sys import argv

rfile = TFile(argv[1])
tree = rfile.Get("bTag_AntiKt4EMTopoJets")
branches = tree.GetListOfBranches()
for branch in branches:
    type_name = branch.GetClassName()
    if not type_name:
        leafs = branch.GetListOfLeaves()
        assert len(leafs) == 1, "can't support multiple leafs in a branch"
        type_name = leafs.At(0).GetTypeName()
    print branch.GetName(), type_name

