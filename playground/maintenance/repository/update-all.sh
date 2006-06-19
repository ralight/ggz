#!/bin/sh

base=$PWD

#svn up
for i in *; do
	if [ -d $i/.svn ]; then
		echo $i
		cd $i
		svn up
		cd $base
	fi
done
