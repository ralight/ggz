#!/bin/sh

rm -rf ggzpostats ggzpots .ggzpostats.files

find . -name *.po -exec echo {} > .ggzpostats.files \;
find . -name *.pot -exec echo {} >> .ggzpostats.files \;

mkdir ggzpostats
for i in `cat .ggzpostats.files`; do
	cp --parents $i ggzpostats;
	#echo $i;
done

find ggzpostats -size 0 -exec rm -f {} \;

cd ggzpostats
pwd=`pwd`
for i in `find * -name *.pot`; do
	dir=`dirname $i`;
	cd $dir;
	echo $dir;
	postats | grep -v "x(100%)x";
	cd $pwd;
done
cd ..

