#!/bin/sh
# Ripped from debianforum.de (thanks chimera)

test -d ~/google
if [ $? != 0 ]
then
  mkdir ~/google > /dev/null
fi

touch ~/google/translations

if ( test -z "$1" ) || ( test -z "$2" ) || ( test -z "$3" )
then
  exit
else
  trans=$1
  lang=`echo $3 | tr '-' '|'` 
  ausdruck=`echo $2 | tr " " "+"` 
  if ( test ! "$1" = "translate" )
  then
    exit
  fi

  grep "$ausdruck" ~/google/translations > /dev/null
if [ $? == 0 ]
  then
   grep "$ausdruck" ~/google/translations | cut -f2
   sleep 1
else
  url="http://translate.google.com/translate_t"
  translated="`lynx -source "$url?text=$ausdruck&langpair=$lang" | cut -f 8 -d "<" | grep ^textarea | cut -c 46-`"
  echo "$translated"
  sleep 1
fi
fi

grep "$ausdruck" ~/google/translations > /dev/null
if [ $? != 0 ]
then
  test -z "$translated"
  if [ $? != 0 ]
   then
    echo -e "$ausdruck\t$translated" >> ~/google/translations
  fi
fi

