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
OUT_BASE=$(basename ${INPUT_FILE%.root*})-$SLURM_ARRAY_TASK_ID
OUTPUT_FILE_HL=${OUT_BASE}-hl.txt.gz
OUTPUT_FILE_IP3D=${OUT_BASE}-ip3d.txt.gz

echo "running on $INPUT_FILE"
mkdir -p $OUTPUT_DIR
btag-dump-jets-hl $INPUT_FILE | gzip > ${OUTPUT_DIR}/$OUTPUT_FILE_HL
btag-dump-jets-ip3d $INPUT_FILE | gzip > ${OUTPUT_DIR}/$OUTPUT_FILE_IP3D

