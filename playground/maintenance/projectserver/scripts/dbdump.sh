#!/bin/sh
# GGZ Gaming Zone Maintenance Script
# Backing up PostgreSQL databases
# Run as user postgres, via cron

mkdir -p ~/dumps

pg_dump ggz > ~/ggz.dump.tmp

mv ~/ggz.dump.tmp ~/dumps/ggz.dump

day=`date +%d`
if test "$day" = "01"; then
        stamp=`date +%Y%m%d`
        cp ~/dumps/ggz.dump ~/dumps/ggz.$stamp.dump
fi

