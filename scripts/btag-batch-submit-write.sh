#!/usr/bin/env bash

set -eu

# default options
ROOT_DIR_LIST=root-dirs.txt
ROOT_DIR=root

_usage() {
    echo "usage: ${0##*/} [-h] [options]"
}
_help() {
    _usage
    cat <<EOF

Submit batch jobs to write hdf5 files for fat jet studies

Will produce a list of root files in the current directory, then
submit one job for each.

Options:
 -h: get help
 -r <root dir>: where the files are, (default: $ROOT_DIR)
 -f <file list>: name for root directory list, (default: $ROOT_DIR_LIST)
 -t: test run, produce list of input files but don't submit

EOF
}

# __________________________________________________________________
# top level run script

RUN_SCRIPT=btag-batch-run-fatwrite.sh
PREFIX=''
WALLTIME=04:00:00

while getopts ":hr:tf:" opt $@; do
    case $opt in
        h) _help; exit 1;;
        r) ROOT_DIR=${OPTARG} ;;
        t) PREFIX=echo ;;
        f) ROOT_DIR_LIST=${OPTARG} ;;
        # handle errors
        \?) _usage; echo "Unknown option: -$OPTARG" >&2; exit 1;;
        :) _usage; echo "Missing argument for -$OPTARG" >&2; exit 1;;
        *) _usage; echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
    esac
done

# -- find the run scripts --
# TODO: simplify this, we can just require an absolute path
RUN_PATH=$(readlink -m $(dirname $0)/$RUN_SCRIPT)
if [[ ! -f $RUN_PATH && ! $PREFIX ]]; then
    echo "$RUN_PATH not found!" >&2
    exit 1
fi

# -- read in some common tools --
# see scripts/batch-tools.sh
TOOLS=$(readlink -e $(dirname $0)/batch-tools.sh)
. $TOOLS

# -- root is stupid: we need to copy .pcm files into this directory
copy-pcm

# -- make a list of files and submit --
if [[ ! -f $ROOT_DIR_LIST ]] ; then
    echo "rebuilding root file list: $ROOT_DIR_LIST"
    ROOT_FILE_LIST=$(mktemp)
    build-root-file-list $ROOT_DIR $ROOT_FILE_LIST
    cat $ROOT_FILE_LIST | sed 's:/[^/]*$::' | sort -u > $ROOT_DIR_LIST
fi

NDIRS=$(wc -l $ROOT_DIR_LIST | cut -d ' ' -f 1)
echo "files in $NDIRS directories"

# setup output
JOB_DIR=runs/output
LOG_DIR=$JOB_DIR/logs
OUT_FILES=$JOB_DIR/results

move-if-exists $JOB_DIR

mkdir -p $LOG_DIR
BOPTS=$(get-slurm-opts $LOG_DIR $WALLTIME)
BOPTS+=" "$(get-array-opts $ROOT_DIR_LIST)
if [[ $PREFIX ]] ; then
    echo -e "** Test mode ** will submit the following line\n"
fi

# submit (and check return code)
${PREFIX} sbatch $BOPTS $RUN_PATH $TOOLS $ROOT_DIR_LIST $OUT_FILES;
echo -e "\noutput will be in '$OUT_FILES'"