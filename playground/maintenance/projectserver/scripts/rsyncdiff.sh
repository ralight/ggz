#!/bin/sh
#
# Script to produce arbitrary binary diff files
# Syntax:
# - for diffing: rsyncdiff.sh <source-dir> <target-dir> [diffname]
# - for patching: ./diffname.sh <target-dir>
# Notes:
# - diffname is 'changeset' by default
# - target-dir will be duplicated temporarily

if [ -z $1 ]; then
	echo "Missing source directory!"
	exit
fi

if [ -z $2 ]; then
	echo "Missing target directory!"
	exit
fi

diffname=changeset
source=$1/
dest=$2/

if [ -n "$3" ]; then
	diffname=$3
fi

desttmp=/tmp/rsyncdiff.$USER.tmp

rm -rf $desttmp
cp -a $dest $desttmp
rsync -avz --write-batch=$diffname $source $desttmp
rm -rf $desttmp

