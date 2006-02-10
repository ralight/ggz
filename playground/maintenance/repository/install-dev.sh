#!/bin/sh

base=/home/ggz/ggz-svn/trunk/
target=/var/www/ggz/www/dev/i18n/
targetart=/var/www/ggz/www/dev/art/

cp $base/postats.txt $target
cp $base/wwwstats.txt $target
cp $base/ggzpots.tar.gz $target

cp $base/artstats.txt $targetart
cp $base/ggzgraphics-*.tar.gz $targetart

