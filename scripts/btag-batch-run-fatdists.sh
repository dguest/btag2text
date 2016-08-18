#!/bin/bash

set -eu

OUTPUT_DIR=$3

echo "submit from $SLURM_SUBMIT_DIR, array index $SLURM_ARRAY_TASK_ID"
cd $SLURM_SUBMIT_DIR

# read in some common tools
. $1

IN_FILE_FILE=$2
if [[ ! -f $IN_FILE_FILE ]]; then
    echo "$IN_FILE_FILE not found!" >&2
    exit 1
fi

INPUT_FILE=$(get-input-file $IN_FILE_FILE)
OUTPUT_PT_ETA=$(get-output-name $INPUT_FILE)

echo "running on $INPUT_FILE"
mkdir -p $OUTPUT_DIR
btag-distributions-pt-eta $INPUT_FILE -o ${OUTPUT_DIR}/$OUTPUT_PT_ETA.h5

