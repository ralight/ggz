#!/bin/sh

version=0.0.14
base=$PWD

prefix=/tmp/ggz-dist-$version
DISTCHECK_CONFIGURE_FLAGS=--prefix=$prefix

echo "GGZ $version Distribution"
for i in *; do
	if test -d $i && test -f $i/Makefile.am; then
		echo $i...
		cd $i
		./autogen.sh --prefix=$prefix >/dev/null
		make distcheck >/dev/null
		cp *-$version.tar.gz ..
		cd $base
	elif test -d $i && test -f $i/Makefile; then
		echo $i...
		cd $i
		if test -f autogen.sh; then
			./autogen.sh --prefix=$prefix >/dev/null
		fi
		make dist >/dev/null
		#cp ../*-$version.tar.gz ..
		cd $base
	fi
done
echo "Tarball creation completed."

