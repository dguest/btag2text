#!/bin/bash

set -eu

INPUT_DIR=$1
OUTPUT_DIR=$2
EXT=$3

# jump to submit dir
cd $SLURM_SUBMIT_DIR

if [[ ! -d $INPUT_DIR ]] ; then
    echo "$DIR not found!" >&2
    exit 1
fi

mkdir -p $OUTPUT_DIR
cat $INPUT_DIR/*$EXT > $OUTPUT_DIR/alljets$EXT

