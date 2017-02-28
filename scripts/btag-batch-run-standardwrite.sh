#!/bin/bash

set -eu

# set the number of tracks to truncate at
N_TRACKS=60

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

echo "running on $INPUT_DIR at $(date)"
mkdir -p $OUTPUT_DIR
OUTPUT_PATH=${OUTPUT_DIR}/$OUTPUT_FILE.h5

INPUT_FILES=$(shopt -s nullglob; echo $INPUT_DIR/*.root*)
if [[ ! $INPUT_FILES ]]; then
    echo "ERROR: no input files" >&2
    exit 1
fi
echo "output path $OUTPUT_PATH"

OPTS="--track-size $N_TRACKS "
btag-write-jets-standard $INPUT_FILES -o $OUTPUT_PATH $OPTS &
PID=$!
trap "kill -TERM $PID; wait $PID; echo killed! >&2" TERM
wait $PID
echo "done at $(date)"

