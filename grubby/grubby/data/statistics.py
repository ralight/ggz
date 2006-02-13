#!/usr/bin/env python

import grubby
import re
import pg

input = grubby.answer
grubby.answer = ""

s = input.split(" ")
if len(s) == 2:
	if s[0] == "stats":
		handle = s[1]

		cns = pg.DB(host="localhost", dbname="ggz", user="ggzd", passwd="ggzd")

		q = cns.query("SELECT * FROM stats WHERE handle = '" + handle + "'")

		for row in q.dictresult():
			game = row['game']
			print game
			grubby.answer += game + "\n"

