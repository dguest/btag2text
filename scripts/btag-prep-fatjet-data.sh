#!/usr/bin/env bash

set -eu

_usage() {
    echo "usage: ${0##*/} [-h] -i <input dir> -o <output dir>"
}
_help() {
    _usage
    cat <<EOF

Combine datasets for h->bb

Options:
 -h: get help
 -i <base>: base dir where files are
 -o <output>: output directory
 -v: verbose
 -f: overwrite old output if it exists, ignore missing files

EOF
}

# __________________________________________________________________
# top level run script

INPUT=''
OUTPUT=''
VERBOSE=''
FORCE=''

while getopts ":hi:o:vf" opt $@; do
    case $opt in
        h) _help; exit 1;;
        i) INPUT=${OPTARG} ;;
        o) OUTPUT=${OPTARG} ;;
        v) VERBOSE=1 ;;
        f) FORCE=1 ;;
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
function msg() {
    if [[ ! $VERBOSE ]]; then return 0; fi
    echo $1
}

TMP=$(mktemp -d)
function cleanup() {
    rm -rf $TMP
}
trap cleanup EXIT

function require() {
    if [[ $FORCE ]]; then
        return 0
    fi
    local FILE
    for FILE in $@; do
        if [[ ! -f $FILE ]] ; then
            echo "ERROR: $FILE is missing" >&2
            return 1
        fi
    done
}

function only-existing() {
    local INPUT
    for INPUT in $@; do
        if [[ -f $INPUT ]] ; then
            echo -n $INPUT" "
        else
            echo "WARNING: $INPUT is missing" >&2
        fi
    done
}

# other h->bb don't use 20 through 22 for some reason
# but 22 (JZ2W) seems to be ok
msg "getting background files..."
REQUIRED_QCD_FILES=$(echo ${INPUT%/}/d3610{22..32}_*.txt.gz)
require $REQUIRED_QCD_FILES
msg "getting signal files..."
REQUIRED_SIG_FILES=$(echo ${INPUT%/}/d301{488..507}_*.txt.gz)
require $REQUIRED_SIG_FILES

QCD_FILES=$(only-existing $REQUIRED_QCD_FILES)
SIG_FILES=$(only-existing $REQUIRED_SIG_FILES)

btag-dump-jet-labels-fatjets > $TMP/labels.txt
btag-dump-jet-labels-fatjets-terse > $TMP/labels.json

# copy over docs
DOCS=$(dirname $0)/../docs/FATJETS.md
cp $DOCS $TMP/README.md

if [[ -d $OUTPUT ]]; then
    if [[ $FORCE ]] ; then
        msg "removing old $OUTPUT"
        rm -r $OUTPUT
    else
        echo "ERROR: $OUTPUT exists already, quitting..." >&2
        exit 1
    fi
fi

cp -r $TMP $OUTPUT

msg "combining backgrounds..."
cat $QCD_FILES > $OUTPUT/background.txt.gz
msg "combining signal..."
cat $SIG_FILES > $OUTPUT/signal.txt.gz
