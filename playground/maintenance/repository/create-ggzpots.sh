#!/bin/sh

rm -rf ggzpots .ggzpots.files ggzpots.tar.gz

find . -name *.pot -exec echo {} > .ggzpots.files \;
find . -name messages.properties -exec echo {} >> .ggzpots.files \;

mkdir ggzpots
for i in `cat .ggzpots.files`; do
	#cp --parents $i ggzpots;
	cp $i ggzpots;
done

tar cvzf ggzpots.tar.gz ggzpots

