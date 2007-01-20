#!/bin/bash

GGZDLOG=/chroot/home/ggz/logs/ggzd.log

grep -E "TABLE_START|TABLE_END" ${GGZDLOG} \
		| sed -e 's/started a new game of .*//' \
		| sed -e 's/START -/START/' \
		| sed -e 's/ - Game of .* started by//' \
		| sed -e 's/ has ended//' \
		| awk '{for(i = 2; i<=NF; i++) printf "%s ", $i; printf "\n"; }' \
		| sed -e 's/END/END  /' \
		> tablelen

