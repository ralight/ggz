#!/usr/bin/env python
# Server for the Hnefatafl game
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import sys
import time
import random
import os, pwd
import re

import ggzdmod
import module_checkers

def hook_state (state):
	print "* state:", str(state)

def hook_join (num, type, name, fd):
	print "* join:", num, type, name, fd
	#print "(Name: " + ggzdmod.getPlayerName(seat) + ")"
	print "fd =", fd
#	net.init(fd)
#	net.sendstring("your turn")

def hook_leave (num, type, name, fd):
	print "* leave:", num, type, name, fd
	#print "(Name: " + ggzdmod.getPlayerName(seat) + ")"

def hook_log (line):
	print "* log:", line

def hook_data (num, type, name, fd):
	print "* data:", num, type, name, fd
	#print "(Name: " + ggzdmod.getPlayerName(seat) + ")"
	print "fd =", fd
#	net.init(fd)

#	line = net.getstring()

def hook_error (arg):
	print "* error:", arg
	sys.exit(-1)

def initggz():
	ggzdmod.setHandler(ggzdmod.EVENT_STATE, hook_state)
	ggzdmod.setHandler(ggzdmod.EVENT_JOIN, hook_join)
	ggzdmod.setHandler(ggzdmod.EVENT_LEAVE, hook_leave)
	ggzdmod.setHandler(ggzdmod.EVENT_LOG, hook_log)
	ggzdmod.setHandler(ggzdmod.EVENT_DATA, hook_data)
	ggzdmod.setHandler(ggzdmod.EVENT_ERROR, hook_error)

def main():
#	global net
#	global xadrez

	print "### launched"

	""" Setup """

#	xadrez = Xadrez()

	print "### go init ggz"

	initggz()
#	net = Network()

	print "### artificial delay"
	k = 0
	for i in range(9):
		for j in range(99999):
			k = k + i + j

	print "### now go loop"

	ggzdmod.connect()
	ggzdmod.mainLoop()

	""" Main loop """

	while 1:
		pass

if __name__ == "__main__":
	main()

