#!/bin/sh

base=$PWD

echo "GGZ 0.0.8 Distribution"
for i in *; do
	if test -d $i && test -f $i/autogen.sh; then
		echo $i...
		cd $i
		./autogen.sh >/dev/null
		make DESTDIR=/tmp/dist-$i distcheck >/dev/null
		cp *.tar.gz ..
		cd $base
	fi
done
echo "Tarball creation completed."

