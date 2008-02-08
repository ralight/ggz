#!/usr/bin/env python
#
# Commands:
# guru stats

import re
import pg

try:
	import grubby
	embedded = True
except:
	embedded = False

if embedded:
	input = grubby.answer
else:
	input = raw_input("")
answer = ""

s = input.split(" ")
if len(s) == 2:
	if s[0] == "stats":
		handle = s[1]

		cns = pg.DB(host="localhost", dbname="ggz", user="ggzd", passwd="ggzd")

		q = cns.query("SELECT * FROM stats WHERE handle = '" + handle + "'")

		for row in q.dictresult():
			game = row['game']
			print game
			answer += game + "\n"

if embedded:
	grubby.answer = answer
else:
	print answer

