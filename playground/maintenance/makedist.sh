#!/bin/sh

version=0.0.9
base=$PWD

echo "GGZ $version Distribution"
for i in *; do
	if test -d $i && test -f $i/autogen.sh; then
		echo $i...
		cd $i
		./autogen.sh >/dev/null
		make DESTDIR=/tmp/dist-$i distcheck >/dev/null
		cp *-$version.tar.gz ..
		cd $base
	fi
done
echo "Tarball creation completed."

