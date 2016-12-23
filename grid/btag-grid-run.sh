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

SCRIPT_DIR=$(dirname $BASH_SOURCE)
WORK_DIR=${SCRIPT_DIR}/..

echo "Inputs: $1"
INPUTS=$(echo $1 | tr "," " ")

if [[ $0 =~ fatjets ]] ; then
    $WORK_DIR/bin/btag-write-jets-fatjets $INPUTS -o jets.h5
elif [[ $0 =~ standard ]] ; then
    $WORK_DIR/bin/btag-write-jets-standard $INPUTS -o jets.h5 -t 40
else
    echo "ERROR: can't match $0 to action" >&2
fi
