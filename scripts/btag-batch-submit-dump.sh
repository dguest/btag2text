#!/usr/bin/env bash

set -eu

ROOT_DIR=root
ROOT_FILE_LIST=root-files.txt
RUN_SCRIPT=btag-batch-run-dump.sh

RUN_PATH=$(readlink -e $(dirname $0)/$RUN_SCRIPT)
if [[ ! -f $RUN_PATH ]]; then
    echo "$RUN_PATH not found!" >&2
    exit 1
fi

# read in some common tools
. $(dirname $0)/batch-tools.sh

BASE_DIR=$1
echo "submit from $BASE_DIR"

# move to the submit dir and do the submitting
cd $BASE_DIR

# make a list of files
build-root-file-list $ROOT_DIR $ROOT_FILE_LIST
NFILES=$(wc -l $ROOT_FILE_LIST | cut -d ' ' -f 1)
#NFILES=1

# setup output
JOB_DIR=runs/txt
OUTPUT_DIR=$JOB_DIR/output
OUT_FILES=$JOB_DIR/txt

move-if-exists $JOB_DIR

mkdir -p $OUTPUT_DIR
BOPTS=$(get-slurm-opts $OUTPUT_DIR)
BOPTS+=" "$(get-array-opts $ROOT_FILE_LIST)
sbatch $BOPTS $RUN_PATH $ROOT_FILE_LIST $OUT_FILES
