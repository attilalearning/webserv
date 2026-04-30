#!/bin/bash

ROOT_DIR="$1"
PARENT_DIR=$(dirname "$ROOT_DIR")

if [[ "$PARENT_DIR" == "/" ]] || [[ "$PARENT_DIR" == "." ]]; then
	echo "ERROR: no parent directory for $ROOT_DIR"
	exit 1
fi

DATA_FILE="permissions_data"

if [ -d "$ROOT_DIR" ]; then
	find "$ROOT_DIR" | sed "s|^$PARENT_DIR/||" | tac >"${PARENT_DIR}/${DATA_FILE}"
else
	echo "ERROR: $ROOT_DIR is not a directory!"
	exit 1
fi
