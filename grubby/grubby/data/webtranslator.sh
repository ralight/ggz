#!/bin/sh
# Ripped from debianforum.de (thanks chimera)
# http://212.184.215.215/~feltel/debianforum/viewtopic.php?t=292
#
# Example:
# grubby translate de-en Dies ist ein sinnloser Satz

dir=~/.ggz/grubby/google

test -d $dir
if [ $? != 0 ]
then
  mkdir -p $dir > /dev/null
fi

touch $dir/translations

read input
i1=`echo $input | cut -d " " -f 1`
i2=`echo $input | cut -d " " -f 2`
i3=`echo $input | cut -d " " -f 3`
i4=`echo $input | cut -d " " -f 4-`

if ( test -z "$i1" ) || ( test -z "$i2" ) || ( test -z "$i3" ) || ( test -z "$i4" )
then
  exit
else
  trans=$i2
  lang=`echo $i3 | tr '-' '|'` 
  ausdruck=`echo $i4 | tr " " "+"` 
  if ( test ! "$i2" = "translate" )
  then
    exit
  fi

  grep "$lang	$ausdruck" $dir/translations > /dev/null
if [ $? == 0 ]
  then
   grep "$lang	$ausdruck" $dir/translations | cut -f3
   sleep 1
else
  url="http://translate.google.com/translate_t"
  translated="`lynx -source "$url?text=$ausdruck&langpair=$lang" | cut -f 8 -d "<" | grep ^textarea | cut -c 46-`"
  echo "$translated"
  sleep 1
fi
fi

grep "$lang	$ausdruck" $dir/translations > /dev/null
if [ $? != 0 ]
then
  test -z "$translated"
  if [ $? != 0 ]
   then
    echo -e "$lang\t$ausdruck\t$translated" >> $dir/translations
  fi
fi

