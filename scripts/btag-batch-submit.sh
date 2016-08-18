#!/usr/bin/env bash

set -eu

_usage() {
    echo "usage: ${0##*/} [-h]"
}
_help() {
    _usage
    cat <<EOF

Submit batch jobs

Options:
 -h: get help
 -s <script name>: command to run in script
 -r <root dir>: where the files are
 -t: test run

EOF
}

# __________________________________________________________________
# top level run script

RUN_SCRIPT=''
ROOT_DIR=root
PREFIX=''

while getopts ":hs:r:t" opt $@; do
	  case $opt in
	      h) _help; exit 1;;
	      s) RUN_SCRIPT=${OPTARG} ;;
        r) ROOT_DIR=${OPTARG} ;;
        t) PREFIX=echo ;;
	      # handle errors
	      \?) _usage; echo "Unknown option: -$OPTARG" >&2; exit 1;;
        :) _usage; echo "Missing argument for -$OPTARG" >&2; exit 1;;
        *) _usage; echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
	  esac
done
shift $(($OPTIND - 1))
ROOT_FILE_LIST=root-files.txt

RUN_PATH=$(readlink -e $(dirname $0)/$RUN_SCRIPT)
if [[ ! -f $RUN_PATH ]]; then
    echo "$RUN_PATH not found!" >&2
    exit 1
fi

# read in some common tools
TOOLS=$(readlink -e $(dirname $0)/batch-tools.sh)
. $TOOLS

# make a list of files
if [[ ! -f $ROOT_FILE_LIST ]] ; then
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
BOPTS=$(get-slurm-opts $LOG_DIR)
BOPTS+=" "$(get-array-opts $ROOT_FILE_LIST)
${PREFIX} sbatch $BOPTS $RUN_PATH $TOOLS $ROOT_FILE_LIST $OUT_FILES

