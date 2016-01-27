#!/usr/bin/env bash

set -eu

# _________________________________________________________________________
# parse inputs

# check inputs
function _usage() {
    echo "usage: ${0##*/} [jets|tracks] <input file>.."
}

# sort args into inputs and draw options
ALL_OPTIONS=()
ALL_INPUTS=()
for input in $@; do
    ISOPT=""
    for option in jets tracks; do
        if [[ $input == $option ]]; then
            ISOPT=1
            ALL_OPTIONS+=$option
        fi
    done
    if [[ -z $ISOPT ]]; then
        ALL_INPUTS+=( $input )
    fi
done

if (( ${#ALL_INPUTS[*]} < 1 )); then
    _usage
    exit 1
fi
# FIXME: make the routines run on all the inputs, not just the first one
INPUT=${ALL_INPUTS[0]}
if [[ ! -f $INPUT ]]; then
    _usage
    echo "ERROR: no file '$INPUT'" >&2
    exit 1
fi

function _drawing() {
    if (( ${#ALL_OPTIONS[*]} == 0 )); then
        return 0
    fi
    local option
    for option in ${ALL_OPTIONS[*]}; do
        if [[ $1 == $option ]]; then
            return 0
        fi
    done
    return 1
}

# _________________________________________________________________________
# various constants

# define inputs
KIN="kinematics.h5"
RW="reweight.h5"
JET_HISTS="jet_hists.h5"
TRACK_HISTS="track_hists.h5"
# dirs
JET_PLOTS="plots/jet"
TRACK_PLOTS="plots/track"

HERE=$(dirname $(which $0))

#define routines
FILL_RW=$HERE/bin/btag-distributions-pt-eta
FILL_OTHER=$HERE/bin/btag-distributions-other
FILL_TRACK=$HERE/bin/btag-distributions-tracks
MAKE_RW=$HERE/scripts/btag-make-reweight-hists.py
DRAW_OTHER=$HERE/scripts/btag-draw-hists.py

# _________________________________________________________________________
# define functions

# checks the to make sure:
# - The first argument exists
# - The first argument is newer than all the following ones
function need_new() {
    if [[ ! -f $1 ]]; then
        return 0
    fi
    local DEP
    for DEP in ${@:2}; do
        if [[ $DEP -nt $1 ]]; then
            return 0
        fi
    done
    return 1
}
# check to see if second arg (a directory) has files that are older
# than the first arg
function need_new_dir() {
    if ! [[ $(ls -A $2) ]]; then
        return 0
    fi
    local OUTPUT
    for OUTPUT in $2/*; do
        if [[ $OUTPUT -ot $1 ]]; then
            return 0
        fi
    done
    return 1
}

# _________________________________________________________________________
# reweighting stuff

# build and draw reweighting hists
if need_new $KIN $INPUT $FILL_RW; then
    echo "filling RW hists"
    $FILL_RW -f $INPUT -o $KIN
fi
if need_new $RW $KIN $MAKE_RW; then
    echo "building RW hists"
    $MAKE_RW $KIN $RW
fi

# _________________________________________________________________________
# draw main histograms

# draw jets
if _drawing jets; then
    function draw_jets() {
        echo "drawing jets..."
        $DRAW_OTHER $JET_HISTS -o $JET_PLOTS
    }
    if need_new $JET_HISTS $INPUT $RW $FILL_OTHER; then
        echo "building jet hists"
        $FILL_OTHER -f $INPUT -o $JET_HISTS
        draw_jets
    fi
    if [[ ! -d $JET_PLOTS ]] || need_new_dir $DRAW_OTHER $JET_PLOTS ; then
        draw_jets
    fi
fi

# draw tracks
if _drawing tracks; then
    function draw_tracks() {
        echo "drawing tracks..."
        $DRAW_OTHER $TRACK_HISTS -o $TRACK_PLOTS $@
    }
    if need_new $TRACK_HISTS $INPUT $RW $FILL_TRACK; then
        echo "building track hists"
        $FILL_TRACK -f $INPUT -o $TRACK_HISTS
        draw_tracks
    fi
    if [[ ! -d $TRACK_PLOTS ]] || need_new_dir $DRAW_OTHER $TRACK_PLOTS ; then
        draw_tracks
    fi
fi
