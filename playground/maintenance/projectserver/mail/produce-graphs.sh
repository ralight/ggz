#!/bin/sh

cd ~/bin/listgraphs

targetdir=/var/www/mail.ggzgamingzone.org/graphs
mkdir -p $targetdir

./create.sh
for i in *.stats; do
	./imager.pl $i
	cp out.png $targetdir/$i.png
	convert -scale 100 out.png preview.png
	cp preview.png $targetdir/preview.$i.png
done

rm -f *.stats *.png

