#!/usr/bin/env bash

set -eu

ROOT_DIR=root
ROOT_FILE_LIST=root-files.txt
RUN_SCRIPT=btag-batch-run-pt-eta.sh
JOB_DIR=runs/job
OUTPUT_DIR=$JOB_DIR/output
OUTPUT_PATTERN=$OUTPUT_DIR/jobout-%j.txt
OUT_FILES=$JOB_DIR/hists

RUN_PATH=$(readlink -e $(dirname $0)/$RUN_SCRIPT)
if [[ ! -f $RUN_PATH ]]; then
    echo "$RUN_PATH not found!" >&2
    exit 1
fi

BASE_DIR=$1
echo "submit from $BASE_DIR"

# move to the submit dir and do the submitting
cd $BASE_DIR

if [[ ! -d $ROOT_DIR ]]; then
    echo "$ROOT_DIR not found!" >&2
    exit 1
fi
if [[ ! -f $ROOT_FILE_LIST ]]; then
    find $ROOT_DIR -type f -name '*.root*' > $ROOT_FILE_LIST
fi
NFILES=$(wc -l $ROOT_FILE_LIST | cut -d ' ' -f 1)
# NFILES=1

function mv_dir() {
    local odir=$1-$2
    if [[ ! -d $odir ]]; then
        mv $1 $odir
    else
        mv_dir $1 $(($2 + 1))
    fi
}
if [[ -d $JOB_DIR ]]; then
    mv_dir $JOB_DIR 1
fi

mkdir -p $OUTPUT_DIR
echo "submitting for $NFILES files"
BOPTS="-t 5 -p atlas_all -c 2 -o $OUTPUT_PATTERN"
sbatch -a 1-${NFILES} $BOPTS $RUN_PATH $ROOT_FILE_LIST $OUT_FILES
