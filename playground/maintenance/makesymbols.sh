#!/bin/sh
#
# Generates symbol map from function declarations in header files
# which can be used with --Wl,--version-script=linker.map to restrict
# list of exported symbols to those in the header file.
#
# FIXME: only one header can be specified right now, i.e. not
# ggz.h and ggz_common.h at the same time!

if [ -z $1 ]; then
	echo "Syntax: makesymbols.sh <headerfile>" >&2
	exit 1
fi

header=$1

rm -f linker.map
symbols=`ctags-exuberant -x --fields=t --c-kinds=p $1 | cut -d " " -f 1`

echo "VERSION {" >> linker.map
echo "global:" >> linker.map
for symbol in $symbols; do
echo "$symbol;" >> linker.map
done
echo "local: *;" >> linker.map
echo "};" >> linker.map

