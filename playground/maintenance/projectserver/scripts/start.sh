#!/bin/sh
# GGZ Gaming Zone Maintenance Script
# Master start script
# Run as user ggz

export PATH=/home/ggz/ggz-usr/bin:$PATH

ggzd
/home/ggz/tracker.pl /home/ggz/server.log &
grubby &
metaserv --logfile /home/ggz/metaserv.log --daemon &
/usr/sbin/xinetd -f ~/xinetd.conf -limit 20 -reuse -filelog ~/.ggz/xinetd.log

