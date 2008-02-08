#!/usr/bin/env python
#
# Commands:
# (none, but listens to certain words instead)

import re

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

rin = re.compile("(\w+) (\w+)[\.\!\,]?")
min = rin.match(input)
if min:
	lang = min.group(1).lower()
	expr = min.group(2).lower()

	if lang == "java":
		if expr == "sucks":
			answer = "True, but it depends on the JVM that is being used."
		if expr == "rocks":
			answer = "The framework is ok, but the GUI simply doesn't cut it."
	if lang == "ruby":
		if expr == "sucks":
			answer = "So what? Stick with Perl if you really need to."
		if expr == "rocks":
			answer = "Sure it does."

	w1 = lang
	w2 = expr

	if w1 == "on":
		if w2 == "windows":
			answer = "In a world without fences, who needs windows or gates?"
		if w2 == "linux":
			answer = "May the source be with you!"
		if w2 == "hurd":
			answer = "Now officially confirmed: The Hurd will come out next year!"

# FIXME: provide nanosleep-based call?
def xsleep(q):
	for j in range(q):
		for i in range(q):
			i *= 2

if answer != "":
	xsleep(1000)

if embedded:
	grubby.answer = answer
else:
	print answer

