#!/bin/bash

# For this to work you need something like:
#
# iptables -N ggz-in
# iptables -N ggz-out
# iptables -A INPUT -i eth0 -p tcp -m tcp --dport 5688 -j ggz-in
# iptables -A INPUT -o eth0 -p tcp -m tcp --sport 5688 -j ggz-out
# iptables -A ggz-in -j ACCEPT
# iptables -A ggz-out -j ACCEPT

LOGDIR=/var/log/ggzd
GGZDLOG=/chroot/home/ggz/logs/ggzd.log

STAMP=$(date +%s)

export GGZIN=`/sbin/iptables -L ggz-in -v -x | grep ACCEPT | awk '{print $1,$2}'`
export GGZOUT=`/sbin/iptables -L ggz-out -v -x | grep ACCEPT | awk '{print $1,$2}'`

echo "${STAMP} ${GGZIN}" >> ${LOGDIR}/ggzin.log
echo "${STAMP} ${GGZOUT}" >> ${LOGDIR}/ggzout.log

/sbin/iptables -Z ggz-in
/sbin/iptables -Z ggz-out

# Log load averages

LOAD=$(cat /proc/loadavg | awk '{print $1,$2,$3}')
echo "${STAMP} ${LOAD}" >> ${LOGDIR}/load.log

# Memory usage

MEM=$(/usr/bin/vmstat | sed -e '1,2d' | awk '{print $3,$4,$5,$6}')
echo "${STAMP} ${MEM}" >> ${LOGDIR}/mem.log

# Player and tables

# Fudge due to missing data
cat ${LOGDIR}/stats_offset.log > ${LOGDIR}/stats.log
grep UPDATE ${GGZDLOG} | awk '{print $7,$8+$9,$8,$9,$10,$11,$12,$13,$14}' >> ${LOGDIR}/stats.log

# Process memory usage
GGZD_PROCESSES=$(ps -U ggz -u ggz u | grep "ggzd -F" | wc -l)
GGZD_MEMORY=$(ps -U ggz -u ggz u | grep "ggzd -F" | tail -n 1 | awk '{print $5}')

echo "${GGZD_PROCESSES} ${GGZD_MEMORY}" >> ${LOGDIR}/ggzdmem.log
