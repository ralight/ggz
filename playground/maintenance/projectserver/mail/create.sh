#!/bin/sh

mailman=/var/lib/mailman/archives/public
base=$PWD

cd $mailman
for list in `ls -1 | grep -v mbox`; do
	echo "*** use list $list"
	cd $list
	rm -f $base/$list.stats
	for i in *.txt; do
		#month=`echo $i | cut -d "." -f 1 | cut -d "-" -f 2`
		month=`echo $i | cut -d "." -f 1`
		num=`grep ^From $i | wc -l`
		num=$[$num / 2]
		echo $month >> $base/$list.stats
		echo $num >> $base/$list.stats
	done
	cd ..
done
cd $base

