#!/bin/bash

set -eu

# set the number of clusters and tracks to truncate at
N_CLUSTERS=60
N_TRACKS=60

# maximum allowed event weight
MAX_WEIGHT=5e4

# =====================================================================
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

INPUT_DIR=$(get-input-file $IN_FILE_FILE)
OUTPUT_FILE=$(get-output-name-from-dir $INPUT_DIR)

echo "running on $INPUT_DIR"
mkdir -p $OUTPUT_DIR
OUTPUT_PATH=${OUTPUT_DIR}/$OUTPUT_FILE.h5

# cross section file lookup
if [[ -f xsec.txt ]] ; then
    XSEC_FILE=xsec.txt
else
    XSEC_FILE=$PY_DIR/../data/xsec.txt
fi
if [[ ! -f $XSEC_FILE ]] ; then
    echo "ERROR: no xsec file found" >&2
    exit 1
fi

# input and output
INPUT_FILES=$(shopt -s nullglob; echo $INPUT_DIR/*.root*)
if [[ ! $INPUT_FILES ]]; then
    echo "ERROR: no input files" >&2
    exit 1
fi
echo "output path $OUTPUT_PATH"
DSID=$(echo $OUTPUT_FILE | sed -r 's/d([0-9]*).*/\1/')
if ! WEIGHT=$(echo $DSID | $PY_DIR/btag-get-xsec.py $XSEC_FILE); then
    echo "WARNING: no cross section data found, quitting"
    exit 1
fi


# rw stuff
RW_FILE=reweight.h5
RW_APP=''
if echo $DSID | $PY_DIR/btag-get-xsec.py -s $XSEC_FILE; then
    if [[ ! -f $RW_FILE ]] ; then
        echo "WARNING: no reweighting file found!"
    else
        RW_APP="--reweight-file ${RW_FILE}"
        echo "using reweighting: ${RW_FILE}"
    fi
else
    RW_APP="--max-weight ${MAX_WEIGHT}"
fi

# run
echo "weighted by $WEIGHT"
OPTS="--track-size $N_TRACKS --cluster-size $N_CLUSTERS ${RW_APP}"
btag-write-jets-fatjets $INPUT_FILES -o $OUTPUT_PATH -w $WEIGHT $OPTS &
PID=$!
trap "kill -TERM $PID; wait $PID; echo killed! >&2" TERM
wait $PID
echo "done"

