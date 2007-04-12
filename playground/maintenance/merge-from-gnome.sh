#!/bin/sh

gnomepolocation=/home/josef/ggz/Gnome-Games/gnome-games-trunk/po
catalog=$1

if [ -z $catalog ]; then
	echo "Syntax error: $0 <potfile>"
	exit 1
fi

for i in $gnomepolocation/*.po; do
	echo "- merge $i"
	filename=$(basename $i)
	echo $filename
	msgmerge $i $catalog > $filename.tmp

	:> $filename
	echo "# This translation was merged from GNOME SVN" >> $filename
	echo "# (http://svn.gnome.org/svn/gnome-games/trunk/po/$filename)" >> $filename
	echo "" >> $filename
	#grep -v "#~" $filename.tmp >> $filename
	csplit $filename.tmp /#~/
	cat xx00 >> $filename
	rm -f xx00 xx01
	rm -f $filename.tmp
done


