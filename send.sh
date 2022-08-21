#!/bin/bash

SCRIPT_DIR=`dirname "$0"`
LOCK_DIR="${LOCK_DIR:-$SCRIPT_DIR}"

flock -w 10 "$LOCK_DIR/send.lock" "$SCRIPT_DIR/send" "$@"
