#!/bin/sh

formats="xpm png svg"

for i in *-*; do
	echo -n "$i";
	for j in $formats; do
		echo -n " $j="
		num=`find $i -name "*.$j" | wc -l;`
		echo -n $num

		find $i -name "*.$j" >> .artlist.$j
	done
	echo ""
done

for j in $formats; do
	mkdir ggzgraphics-$j;
	for i in `cat .artlist.$j`; do
		cp --parent $i ggzgraphics-$j;
	done
	tar czf ggzgraphics-$j.tar.gz ggzgraphics-$j;

	rm -rf ggzgraphics-$j;
	rm -f .artlist.$j;
done

