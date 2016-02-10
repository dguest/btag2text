#!/usr/bin/env bash

set -eu

# read in some common tools
. $(dirname $0)/batch-tools.sh

RUN_SCRIPT=btag-batch-run-textcat.sh
RUN_PATH=$(readlink -e $(dirname $0)/$RUN_SCRIPT)
if [[ ! -f $RUN_PATH ]]; then
    echo "$RUN_PATH not found!" >&2
    exit 1
fi


# move to the submit dir and do the submitting
BASE_DIR=$1
echo "submit from $BASE_DIR"
cd $BASE_DIR

# setup output
INPUT_DIR=runs/txt/txt
JOB_DIR=runs/cat
OUTPUT_DIR=$JOB_DIR/output
OUT_FILES=$JOB_DIR/cat

# check inputs, setup outputs
if [[ ! -d $INPUT_DIR ]]; then
    echo "input directory $INPUT_DIR not found" >&2
    exit 1
fi
move-if-exists $JOB_DIR
mkdir -p $OUTPUT_DIR

BOPTS=$(get-slurm-opts $OUTPUT_DIR 02:00:00)
sbatch $BOPTS $RUN_PATH $INPUT_DIR $OUT_FILES -hl.txt.gz
sbatch $BOPTS $RUN_PATH $INPUT_DIR $OUT_FILES -ip3d.txt.gz

