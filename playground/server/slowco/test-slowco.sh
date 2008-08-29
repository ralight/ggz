#!/bin/sh
# derived from tutorial by pipitas

# the interface
iface=lo

# end of config section

# display discipline
/sbin/tc qdisc show dev $iface

# test
ping -c 3 127.0.0.1

# add 20 ms roundtrip delay
/sbin/tc qdisc add dev $iface root handle 1:0 netem delay 20msec

# display again, should now be changed
/sbin/tc qdisc show dev $iface

# test with changed settings
ping -c 3 127.0.0.1

# delete again
/sbin/tc qdisc del dev $iface root

