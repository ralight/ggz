#!/usr/bin/env python

import grubby
import re

input = grubby.answer
grubby.answer = ""

rin = re.compile("(\w+) (\w+)[\.\!\,]?")
min = rin.match(input)
if min:
	lang = min.group(1).lower()
	expr = min.group(2).lower()

	if lang == "java":
		if expr == "sucks":
			grubby.answer = "True, but it depends on the JVM that is being used."
		if expr == "rocks":
			grubby.answer = "The framework is ok, but the GUI simply doesn't cut it."
	if lang == "ruby":
		if expr == "sucks":
			grubby.answer = "So what? Stick with Perl if you really need to."
		if expr == "rocks":
			grubby.answer = "Sure it does."

	w1 = lang
	w2 = expr

	if w1 == "on":
		if w2 == "windows":
			grubby.answer = "In a world without fences, who needs windows or gates?"
		if w2 == "linux":
			grubby.answer = "May the source be with you!"
		if w2 == "hurd":
			grubby.answer = "Now officially confirmed: The Hurd will come out next year!"

def xsleep(q):
	for j in range(q):
		for i in range(q):
			i *= 2

if grubby.answer != "":
	xsleep(1000)

