#!/bin/sh
# Maintenance script
# Create GGZ message catalog archive and statistics file

cd $HOME/ggz-cvs/current
./update-all.sh
./create-ggzpots.sh
./statistics-ggzpos.sh > postats.txt
./statistics-webpages.sh > wwwstats.txt

