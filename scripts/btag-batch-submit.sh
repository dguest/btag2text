#!/usr/bin/env bash

set -eu

# default options
ROOT_FILE_LIST=root-files.txt
ROOT_DIR=root

_usage() {
    echo "usage: ${0##*/} [-h] [options]"
}
_help() {
    _usage
    cat <<EOF

Submit batch jobs

Will produce a list of root files in the current directory, then
submit one job for each.

Options:
 -h: get help
 -s <script name>: command to run in script
 -r <root dir>: where the files are, (default: $ROOT_DIR)
 -f <file list>: name for root file list, (default: $ROOT_FILE_LIST)
 -t: test run, produce list of input files but don't submit
 -w <walltime>: formatted as HH:MM:SS

EOF
}

# __________________________________________________________________
# top level run script

RUN_SCRIPT=''
PREFIX=''
WALLTIME=00:45:00

while getopts ":hs:r:tw:f:" opt $@; do
    case $opt in
        h) _help; exit 1;;
        s) RUN_SCRIPT=${OPTARG} ;;
        r) ROOT_DIR=${OPTARG} ;;
        t) PREFIX=echo ;;
        w) WALLTIME=${OPTARG} ;;
        f) ROOT_FILE_LIST=${OPTARG} ;;
        # handle errors
        \?) _usage; echo "Unknown option: -$OPTARG" >&2; exit 1;;
        :) _usage; echo "Missing argument for -$OPTARG" >&2; exit 1;;
        *) _usage; echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
    esac
done
# check for errors in configuration
if [[ ! $RUN_SCRIPT && ! $PREFIX ]] ; then
    _usage
    echo "no script given, quitting!" >&2
    exit 1
fi

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
if [[ ! -f $ROOT_FILE_LIST ]] ; then
    echo "rebuilding root file list: $ROOT_FILE_LIST"
    build-root-file-list $ROOT_DIR $ROOT_FILE_LIST
fi
NFILES=$(wc -l $ROOT_FILE_LIST | cut -d ' ' -f 1)
#NFILES=1

# setup output
JOB_DIR=runs/output
LOG_DIR=$JOB_DIR/logs
OUT_FILES=$JOB_DIR/results

move-if-exists $JOB_DIR

mkdir -p $LOG_DIR
BOPTS=$(get-slurm-opts $LOG_DIR $WALLTIME)
BOPTS+=" "$(get-array-opts $ROOT_FILE_LIST)
if [[ $PREFIX ]] ; then
    echo -e "** Test mode ** will submit the following line\n"
fi

# submit (and check return code)
${PREFIX} sbatch $BOPTS $RUN_PATH $TOOLS $ROOT_FILE_LIST $OUT_FILES;
echo -e "\noutput will be in '$OUT_FILES'"
