#!/bin/sh
# GGZ Gaming Zone Maintenance Script
# Master start script
# Run as user ggz

export PATH=/home/ggz/ggz-usr/bin:$PATH
export PYTHONPATH=/home/ggz/ggz-usr

ulimit -c 1024

# safety first!
killall xinetd metaserv tracker.pl grubby ggzd 2>/dev/null

ggzd
/home/ggz/tracker.pl /home/ggz/server.log &
grubby &
metaserv --logfile /home/ggz/metaserv.log --daemon &
/usr/sbin/xinetd -f ~/xinetd.conf -limit 20 -reuse -filelog ~/.ggz/xinetd.log

