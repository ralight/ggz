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
	csplit $filename.tmp /#~/ >/dev/null
	cat xx00 >> $filename
	rm -f xx00 xx01
	rm -f $filename.tmp

	lastline=`tail -1 $filename`
	if [ ! -z "$lastline" ]; then
		numlines=`wc -l $filename | cut -d " " -f 1`
		numlines=$[$numlines - 1]
		head -$numlines $filename > $filename.tmp
		mv $filename.tmp $filename
	fi

	numtranslated=`potool -ft -fnf -s $filename`
	if [ $numtranslated -eq "1" ]; then
		rm -f $filename
		svn remove $filename >/dev/null
	fi
done


