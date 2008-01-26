#!/usr/bin/env python
#
# Commands:
# guru joke
# guru fortune
# guru fortune linux
# guru fortune de/mathematiker

try:
	import grubby
	embedded = True
except:
	embedded = False
import os

if embedded:
	input = grubby.answer
else:
	input = raw_input("")
answer = ""

inp = input.split(" ")
if len(inp) == 2 or len(inp) == 3:
	command = inp[1]
	category = ""
	if len(inp) == 3:
		category = inp[2]
	if command == "joke" or command == "fortune":
		f = os.popen("fortune " + category)
		lines = f.readlines()
		f.close()

		xlines = ""
		for line in lines:
			xlines += line

		answer = xlines

if embedded:
	grubby.answer = answer
else:
	print answer

