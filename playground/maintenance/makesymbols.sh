#!/bin/sh
#
# Generates symbol map from function declarations in header files
# which can be used with --Wl,--version-script=linker.map to restrict
# list of exported symbols to those in the header file.

if [ -z $1 ]; then
	echo "Syntax: makesymbols.sh <headerfile(s)>" >&2
	exit 1
fi

symbols=""
headers=""
lb=`echo -e '\n '`
while [ ! -z $1 ]; do
	header=$1
	newsymbols=`ctags-exuberant -x -I ggz__attribute --c-kinds=p $1 | cut -d " " -f 1`
	symbols="$symbols$lb$newsymbols"
	headers="$headers $header"
	shift
done

rm -f linker.map

echo "/*" >> linker.map
echo "This linker map has been produced from the following files:" >> linker.map
echo $headers >> linker.map
echo "But it was certainly modified, so check twice!" >> linker.map
echo "*/" >> linker.map
echo "" >> linker.map

#echo "VERSION" >> linker.map
echo "{" >> linker.map
echo "global:" >> linker.map
for symbol in $symbols; do
echo "    $symbol;" >> linker.map
done
echo "local: *;" >> linker.map
echo "};" >> linker.map

