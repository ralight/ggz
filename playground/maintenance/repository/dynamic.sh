#!/bin/sh
# Maintenance script
# Create GGZ message catalog archive and statistics file

cd $HOME/ggz-svn/trunk
echo "update sources"
./update-all.sh
echo "create translation catalog pack"
./create-ggzpots.sh
echo "generate statistics and create art pack"
./statistics-art.sh > artstats.txt
./statistics-ggzpos.sh > postats.txt
./statistics-webpages.sh > wwwstats.txt

