#!/bin/bash

GREPSTRING=NEWACCT
TMPFILE=accts.txt
OUTFILE=accts_count.txt
GGZDLOG=/chroot/home/ggz/logs/ggzd.log

grep ${GREPSTRING} ${GGZDLOG} | awk '{print $2,$3,$4}' > ${TMPFILE}

started=0
let missed=0
rm -f ${OUTFILE}
for year in $(seq 2006 2007); do
	for month in Jan Feb Mar Apr Jun Jul Aug Sep Oct Nov Dec; do
		for day in $(seq -w 1 31); do
#			echo "${month} ${day} ${year}"
			count=$(grep -c "${month} ${day} ${year}" ${TMPFILE})
			result=$?
			if [ "${result}" == "1" ]; then
				if [ "${started}" == "1" ]; then
					let missed=${missed}+1
					if [ "${missed}" == "5" ]; then
						rm -f ${TMPFILE}
						head -n -5 ${OUTFILE}.tmp > ${OUTFILE}
						rm -f ${OUTFILE}.tmp
						exit
					fi
				fi
			else
				started=1
				let missed=0
			fi
			if [ "${started}" == "1" ]; then
				echo "${day}_${month}_${year} ${count}" >> ${OUTFILE}.tmp
			fi
		done
	done
done
