#!/usr/bin/env python

import grubby
import os

input = grubby.answer
grubby.answer = ""

inp = input.split(" ")
if len(inp) == 2 or len(inp) == 3:
	command = inp[1]
	category = "riddles"
	if len(inp) == 3:
		category = inp[2]
	if command == "joke" or command == "fortune":
		f = os.popen("fortune " + category)
		lines = f.readlines()
		f.close()

		xlines = ""
		for line in lines:
			xlines += line

		grubby.answer = xlines

