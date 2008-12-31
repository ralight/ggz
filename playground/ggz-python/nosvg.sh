#!/bin/sh
#
# Pre-rendering of PNG files
# Note: We assume we can scale them correctly later. Otherwise, we'd need
# them prerendered in multiple sizes. But 50x50 is ggzboard standard size.

for i in `find . -name "*.svg"`
do
	echo -n $i
	#for j in 16 24 32 48
	for j in 50
	do
		echo -n "."
		#convert -scale $j $i $i.$j.png
		convert -scale $j $i $i.png
	done
	echo ""
done

