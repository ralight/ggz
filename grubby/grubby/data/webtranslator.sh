#!/bin/sh
# Ripped from debianforum.de (thanks chimera)
# http://212.184.215.215/~feltel/debianforum/viewtopic.php?t=292

dir=~/.ggz/grubby/google

test -d $dir
if [ $? != 0 ]
then
  mkdir -p $dir > /dev/null
fi

touch $dir/translations

if ( test -z "$1" ) || ( test -z "$2" ) || ( test -z "$3" ) || ( test -z "$4" )
then
  exit
else
  trans=$2
  lang=`echo $4 | tr '-' '|'` 
  ausdruck=`echo $3 | tr " " "+"` 
  if ( test ! "$2" = "translate" )
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

