#!/bin/sh

langs="en de pt fr"

for lang in $langs; do
	val=`find /var/www/ggz/www -name "*.php.$lang" | wc -l`
	val=`echo $val | tr -s " "`
	echo "$lang $val"
done

