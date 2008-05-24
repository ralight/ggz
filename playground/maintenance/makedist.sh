#!/bin/sh

version=0.0.14
base=$PWD
log=makedist.log

prefix=/tmp/ggz-dist-$version
export DISTCHECK_CONFIGURE_FLAGS=--prefix=$prefix

modules="libggz ggz-client-libs ggzd docs txt-client utils grubby gtk-client gtk-games kde-client kde-games sdl-games gnome-client java python community"

echo "GGZ $version Distribution"
for i in $modules; do
	if test -d $i && test -f $i/Makefile.am; then
		echo $i...
		cd $i
		./autogen.sh --prefix=$prefix >$log
		make distcheck >$log
		make >$log
		make install >$log
		cp *-$version.tar.gz ..
		cd $base
	elif test -d $i && (test -f $i/Makefile || test -f $i/Makefile.in); then
		echo $i...
		cd $i
		if test -f autogen.sh; then
			./autogen.sh --prefix=$prefix >$log
			make >$log
			make install >$log
		fi
		make dist >$log
		#cp ../*-$version.tar.gz ..
		cd $base
	fi
done
echo "Tarball creation completed."

