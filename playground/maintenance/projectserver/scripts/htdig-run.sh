#!/bin/sh
# GGZ Gaming Zone Maintenance Script
# Creation of htdig search index

base=/var/www/ggz/mail

echo Temporarily disabling all restrictions...
mv $base/robots.txt $base/htdig.robots.txt
mv $base/hypermail/.htaccess $base/hypermail/htdig.htaccess
mv $base/pipermail/.htaccess $base/pipermail/htdig.htaccess

echo Generating search database...
htdig -v

echo Moving back all restrictions...
mv $base/htdig.robots.txt $base/robots.txt
mv $base/hypermail/htdig.htaccess $base/hypermail/.htaccess
mv $base/pipermail/htdig.htaccess $base/pipermail/.htaccess

