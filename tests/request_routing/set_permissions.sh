#!/bin/bash

SAME_RIGHTS=
DATA_FILE="permissions_data"
CUSTOM_DATA_FILE=
TARGET_DIR="$1"
PARENT_DIR=$(dirname "$TARGET_DIR")
ERROR_IN_EXECUTION=

show_usage() {
	echo "$0 target_dir [mode|data_file]"
	echo
	echo "target_dir        A directory that needs to contain the"
	echo "                  permissions_data file in it's parent directory!"
	echo
	echo "data_file         If data_file is provided, then data_file needs"
	echo "                  to to exist instead of permissions_data, at the"
	echo "                  path provided in data_file."
	echo
	echo "mode              The mode to set each file and directory to, for"
	echo "                  target_dir and all it's subfolders and files."
	echo
}

if [ -z "$1" ]; then
	show_usage
	exit 1
fi

if [ ! -d "$TARGET_DIR" ]; then
	echo "ERROR: $TARGET_DIR is not a directory!"
	exit 1
fi

if [ ! -z "$2" ] && [[ $2 =~ ^[0-9]{3}$ ]]; then
	SAME_RIGHTS="$2"
elif [ ! -z "$2" ] && [ -f "$2" ]; then
	DATA_FILE="$2"
	CUSTOM_DATA_FILE=1
fi

if [ -z "$CUSTOM_DATA_FILE" ] && [ ! -f "$PARENT_DIR/$DATA_FILE" ]; then
	echo "ERROR: could not find $PARENT_DIR/$DATA_FILE"
	exit 1
fi

if [ ! -z "$CUSTOM_DATA_FILE" ] && [ ! -f "$DATA_FILE" ]; then
	echo "ERROR: could not find $DATA_FILE"
	exit 1
fi

if [ -z "$CUSTOM_DATA_FILE" ]; then
	DATA_FILE="$PARENT_DIR/$DATA_FILE"
fi

while IFS=" " read -r path_from_file rights_from_file; do
	rights="$rights_from_file"
	filename="$PARENT_DIR/$path_from_file"

	if [ ! -z "$SAME_RIGHTS" ]; then
		rights="$SAME_RIGHTS"
	fi

	if [ -f "$filename" ] || [ -d "$filename" ]; then
		chmod "$rights" "$filename"
		echo "OK: $rights -> $path_from_file"
	else
		echo "ERROR: $rights .. $path_from_file - not a file, nor a directory!"
		ERROR_IN_EXECUTION=1
	fi
done <"$DATA_FILE"

if [ ! -z $ERROR_IN_EXECUTION ]; then
	exit 2
fi
