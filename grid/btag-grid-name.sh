#!/usr/bin/env bash

# Script to figure out the voms and output file nameing options to
# submit with. Removes unneeded crap from the input file name to get
# the output name and checks for `production` roll to find the submit
# rights.

if [[ $- == *i* ]] ; then
    echo "Don't source me!" >&2
    return 1
else
    set -eu
fi

DS=$1
TAG=${2-}

# -- check the voms --
if ! VOMS=$(voms-proxy-info --vo 2> /dev/null) || [[ -z $VOMS ]]; then
    echo "ERROR: voms-proxy-init not run, quitting" >&2
    exit 1
fi
# if there is one, start with user opts
SCOPE=user.${RUCIO_ACCOUNT-$USER}
OFFICIAL_OPTS=""
# ... then check for production role
PR='Role=production'
if FQAN=$(voms-proxy-info --fqan 2> /dev/null | grep $PR) ; then
    SCOPE=group.$(echo $FQAN | cut -d / -f 3)
    OFFICIAL_OPTS=" --official --voms ${VOMS}:${FQAN%/*}"
fi

# -- figure out new DS name --
# take the DSID
DSID=$(echo $DS | sed -r 's/.*\.([0-9]{6,})\..*/\1/')
GITT=$(git describe)
APP=""
if [[ -n $TAG ]] ; then
    APP=.${TAG}
fi
OUT=${SCOPE}.${DSID}.${GITT}${APP}

# possibly cut down the size further
function too_long() {
    if (( $(echo $OUT | wc -c) > 115 )); then
        return 0
    fi
    return 1
}
if too_long ; then
    echo "ERROR: ds name $OUT is too long" >&2
    exit 1
fi

echo --outDS ${OUT}${OFFICIAL_OPTS}
