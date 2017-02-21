#!/bin/bash

set -eu

OUTPUT_DIR=$3

echo "submit from $SLURM_SUBMIT_DIR, array index $SLURM_ARRAY_TASK_ID"
cd $SLURM_SUBMIT_DIR

# read in some common tools
. $1
PY_DIR=$(dirname $1)

IN_FILE_FILE=$2
if [[ ! -f $IN_FILE_FILE ]]; then
    echo "$IN_FILE_FILE not found!" >&2
    exit 1
fi

INPUT_FILE=$(get-input-file $IN_FILE_FILE)
OUTPUT_FILE=$(get-output-name $INPUT_FILE)

echo "running on $INPUT_FILE"
mkdir -p $OUTPUT_DIR
OUTPUT_PATH=${OUTPUT_DIR}/$OUTPUT_FILE.txt.gz
if [[ -f xsec.txt ]] ; then
    XSEC_FILE=xsec.txt
else
    XSEC_FILE=$PY_DIR/../data/xsec.txt
fi
if [[ ! -f $XSEC_FILE ]] ; then
    echo "ERROR: no xsec file found" >&2
    exit 1
fi
DSID=$(echo $OUTPUT_FILE | sed -r 's/d([0-9]*).*/\1/')
if ! WEIGHT=$(echo $DSID | $PY_DIR/btag-get-xsec.py $XSEC_FILE) ; then
    echo "WARNING: no cross section data found, quitting"
    exit 1
fi
echo "weighted by $WEIGHT"
btag-dump-jets-fatjets $INPUT_FILE -w $WEIGHT | gzip - > $OUTPUT_PATH
