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

IN_FILE_ARRAY=( $(cat $IN_FILE_FILE) )
echo "${#IN_FILE_ARRAY[*]} files in array"
INPUT_FILE=${IN_FILE_ARRAY[$SLURM_ARRAY_TASK_ID-1]}
OUT_BASE=$(basename ${INPUT_FILE%.root*})-$SLURM_ARRAY_TASK_ID
OUTPUT_FILE=${OUT_BASE}-pt-eta.h5

echo "running on $INPUT_FILE"
mkdir -p $OUTPUT_DIR
btag-distributions-pt-eta $INPUT_FILE -o ${OUTPUT_DIR}/$OUTPUT_FILE

