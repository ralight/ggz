#!/bin/sh

formats="xpm png svg"

for i in *; do
	if test -d $i; then
		echo -n "$i";
		for j in $formats; do
			echo -n " $j="
			num=`find $i -name "*.$j" | wc -l;`
			echo -n $num

			find $i -name "*.$j" >> .artlist.$j
		done
		echo ""
	fi
done

for j in $formats; do
	mkdir ggzgraphics-$j;
	for i in `cat .artlist.$j`; do
		if test `stat -c %s $i` -lt 50000; then
			cp --parent $i ggzgraphics-$j;
		fi
	done
	rm -f ggzgraphics-$j.tar.gz;
	tar czf ggzgraphics-$j.tar.gz ggzgraphics-$j;

	rm -rf ggzgraphics-$j;
	rm -f .artlist.$j;
done

