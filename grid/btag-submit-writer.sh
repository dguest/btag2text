#!/usr/bin/env bash

if [[ $- == *i* ]] ; then
    echo "Don't source me!" >&2
    return 1
else
    set -eu
fi

# defaults
DS=''

_usage() {
    echo "usage: ${0##*/} [-h] [options]"
}
_help() {
    _usage
    cat <<EOF

Submit job over some dataset. Internally figures out the output
dataset name and special permissions (based on the options you setup
with voms).

Note that this should be run from the directory where your job options
live.

Requires that:
 - You're in a git-controlled directory
 - All changes have been committed

Options:
 -h: get help
 -n <number>: n files to use (default all)
 -d <dataset>: input dataset to use (default ${DS})
 -t <tag>: tag for output dataset
 -z <file>: create / submit a gziped tarball
 -e: test run, just echo command
 -f: force submit even if uncommited changes exist

EOF
}

OPTS=""
ECHO=""
TAG=""
ZIP=""
FORCE=""
while getopts ":hn:d:t:z:uef" opt $@; do
    case $opt in
        h) _help; exit 1;;
        n) OPTS+=" --nFiles ${OPTARG}";;
        d) DS=${OPTARG};;
        t) TAG=${OPTARG};;
        z) ZIP=${OPTARG};;
        e) ECHO=echo;;
        f) FORCE=1;;
        # handle errors
        \?) _usage; echo "Unknown option: -$OPTARG" >&2; exit 1;;
        :) _usage; echo "Missing argument for -$OPTARG" >&2; exit 1;;
        *) _usage; echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
    esac
done

if ! git diff-index --quiet HEAD && [[ ! $FORCE ]]; then
    echo "ERROR: uncommitted changes in local area, please commit them" >&2
    exit 1
fi

SCRIPT_DIR=$(dirname $BASH_SOURCE)
OUT_OPTS=$(${SCRIPT_DIR}/btag-grid-name.sh $DS $TAG)
WORK_DIR=${SCRIPT_DIR}/..

# setup options
OPTS=${OUT_OPTS}
# OPTS+=" --nFilesPerJob 5"
OPTS+=" --inDS $DS"
OPTS+=" --workDir $WORK_DIR"
OPTS+=" --bexec ./grid/btag-build-writer.sh"
OPTS+=" --outputs out.h5"
EXCLUDE=./bin/,build/,ndhist/build/,ndhist/lib/,ndhist-python/\*,
EXCLUDE+=ndhist/test
OPTS+=" --excludeFile=$EXCLUDE"

EXEC_STR="./grid/btag-run-writer.sh %IN"

# pack stuff into a tarball before submitting
if [[ -n $ZIP ]] ; then
    if [[ ! -f $ZIP ]]; then
        echo "making tarball of local files: ${ZIP}" >&2
        prun --outTarBall=${ZIP} $OPTS --noSubmit --exec "$EXEC_STR"
    fi
    OPTS+=" --inTarBall=${ZIP}"
fi

echo "submitting over dataset ${DS}" >&2

${ECHO} prun $OPTS --exec "$EXEC_STR"

