#!/usr/bin/env bash

set -eu

_usage() {
    echo "usage: ${0##*/} [-h] -i <input dir> -o <output file>"
}
_help() {
    _usage
    cat <<EOF

Hadd hists (for h->bb)

Options:
 -h: get help
 -i <base>: base dir where files are
 -o <output>: output file name (should end in .h5)

EOF
}

# __________________________________________________________________
# top level run script

INPUT=''
OUTPUT=''

while getopts ":hi:o:" opt $@; do
	  case $opt in
	      h) _help; exit 1;;
	      i) INPUT=${OPTARG} ;;
        o) OUTPUT=${OPTARG} ;;
	      # handle errors
	      \?) _usage; echo "Unknown option: -$OPTARG" >&2; exit 1;;
        :) _usage; echo "Missing argument for -$OPTARG" >&2; exit 1;;
        *) _usage; echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
	  esac
done
if [[ -z $INPUT || -z $OUTPUT ]] ; then
    echo "ERROR: -i and -o are required" >&2
    exit 1
fi

TMP=$(mktemp -d)
function cleanup() {
    rm -rf $TMP
}
trap cleanup EXIT

function require() {
    local FILE
    for FILE in $@; do
        if [[ ! -f $FILE ]] ; then
            echo "ERROR: $FILE is missing" >&2
            return 1
        fi
    done
}

QCD_FILES=$(echo $INPUT/3610{20..32}.h5)
require $QCD_FILES
SIG_FILES=$(echo $INPUT/301{488..507}.h5)
require $SIG_FILES

btag-hadd.py $QCD_FILES -o $TMP/qcd.h5
btag-hadd.py $SIG_FILES -o $TMP/hbb.h5
btag-merge-hists.py $TMP/qcd.h5,qcd $TMP/hbb.h5,signal -o $OUTPUT
