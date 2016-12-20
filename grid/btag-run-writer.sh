#!/usr/bin/env bash

if [[ $- == *i* ]] ; then
    echo "Don't source me!" >&2
    return 1
else
    set -eu
fi

if [[ $# < 1 ]]; then
    echo "ERROR: no files given!" >&2
    exit 1
fi

echo "Inputs: $1"
INPUTS=$(echo $1 | tr "," " ")
./bin/btag-write-jets-fatjets $INPUTS -o out.h5
