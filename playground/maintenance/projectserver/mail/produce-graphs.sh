#!/bin/sh

cd ~/listgraphs

./create.sh
for i in *.stats; do
	./imager.pl $i
	cp out.png /var/www/ggz/mail/graphs/$i.png
	convert -scale 100 out.png preview.png
	cp preview.png /var/www/ggz/mail/graphs/preview.$i.png
done

