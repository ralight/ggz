#!/bin/bash

GGZDLOG=/chroot/home/ggz/logs/ggzd.log

grep "LOGOUT" ${GGZDLOG} \
		| awk '{print $10}' \
		| grep -v for \
		| sort -n \
		> loginlen

