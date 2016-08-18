# this is a -*- bash -*- repository for common bash functions that may
# be useful in batch submitting / running

# ________________________________________________________________________
# functions for submission

# function to avoid overwriting the output
function _mv_dir() {
    local odir=$1-$2
    if [[ ! -d $odir ]]; then
        mv $1 $odir
    else
        _mv_dir $1 $(($2 + 1))
    fi
}
function move-if-exists() {
    if [[ -d $1 ]]; then
        _mv_dir $1 1
    fi
}

# build file lists
function build-root-file-list() {
    local ROOT_DIR=$1
    local ROOT_FILE_LIST=$2
    if [[ ! -d $ROOT_DIR ]]; then
        echo "$ROOT_DIR not found!" >&2
        return 1
    fi
    if [[ ! -f $ROOT_FILE_LIST ]]; then
        find $ROOT_DIR -type f -name '*.root*' > $ROOT_FILE_LIST
    fi
    return 0
}

# ________________________________________________________________________
# batch option functions
# arg 1: list of root files
function get-array-opts() {
    local FILE_LIST=$1
    local NFILES=$(wc -l $FILE_LIST | cut -d ' ' -f 1)
    echo "-a 1-${NFILES}"
}
# arg 1: directory for output
# arg 2: run time
function get-slurm-opts() {
    local OUTPUT_DIR=$1
    local RUN_TIME=${2-00:05:00}
    local OUTPUT_PAT=$OUTPUT_DIR/stdout-%j.txt
    local ERROR_PAT=$OUTPUT_DIR/stderr-%j.txt
    echo "-t $RUN_TIME -p atlas_all -c 2 -o $OUTPUT_PAT -e $ERROR_PAT"
}
# _______________________________________________________________________
# functions for run scripts

# Get list of input files
# arg 1: list of input files
function get-input-file() {
    local IN_FILE_FILE=$1

    if [[ ! -f $IN_FILE_FILE ]]; then
        echo "$IN_FILE_FILE not found!" >&2
        return 1
    fi
    declare -a IN_FILE_ARRAY=( $(cat $IN_FILE_FILE) )
    local INPUT_FILE=${IN_FILE_ARRAY[$SLURM_ARRAY_TASK_ID-1]}
    echo $INPUT_FILE
}

# Get the dsid and the file number from a rucio dataset
DS_RE='s/[^\d]*\.([0-9]{6})\..*/\1/'
IDX_RE='s/.*\._([0-9]{6})\.root.*/\1/'
function get-output-name() {
    local INPUT_DIR=$(dirname $1)
    local INPUT_FILE=$(basename $1)
    local DS=$(echo $INPUT_DIR | sed -r $DS_RE)
    local IDX=$(echo $INPUT_FILE | sed -r $IDX_RE)
    echo $DS.$IDX
}
