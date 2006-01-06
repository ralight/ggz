#!/usr/bin/env python
# Server for the Arimaa game
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import sys
import time
import random
import os, pwd
import re

import gettext
gettext.install("ggzpython", None, 1)

import ggzdmod
from module_arimaa import *
from ggzboard_net import *

class Server(NetworkBase):
	def __init__(self):
		NetworkBase.__init__(self)
		self.MSG_SEAT = 0
		self.MSG_PLAYERS = 1
		self.MSG_START = 5

		self.MSG_MOVE = 2
		self.MSG_GAMEOVER = 3
		self.REQ_MOVE = 4

	def table_full(self):
		full = 1
		for i in range(ggzdmod.getNumSeats()):
			name = ggzdmod.seatName(i)
			type = ggzdmod.seatType(i)
			fd = ggzdmod.seatFd(i)
			if type != ggzdmod.SEAT_PLAYER and type != ggzdmod.SEAT_BOT:
				full = 0
		return full

	def send_players(self):
		for i in range(ggzdmod.getNumSeats()):
			fd = ggzdmod.seatFd(i)
			if fd != -1:
				net.init(fd)
				net.sendbyte(self.MSG_SEAT)
				net.sendbyte(i)
				net.sendbyte(self.MSG_PLAYERS)
				for j in range(ggzdmod.getNumSeats()):
					type = ggzdmod.seatType(j)
					print "TYPE", type
					net.sendbyte(type)
					if type != ggzdmod.SEAT_OPEN:
						name = ggzdmod.seatName(j)
						print "NAME", name
						net.sendstring(name)

	def send_start(self):
		for i in range(ggzdmod.getNumSeats()):
			fd = ggzdmod.seatFd(i)
			if fd != -1:
				net.init(fd)
				net.sendbyte(self.MSG_START)

def hook_state (state):
	print "* state:", str(state)

def hook_join (num, type, name, fd):
	print "* join:", num, type, name, fd
	net.send_players()
	if net.table_full():
		net.send_start()

def hook_leave (num, type, name, fd):
	print "* leave:", num, type, name, fd

def hook_log (line):
	print "* log:", line

def hook_data (num, type, name, fd):
	print "* data:", num, type, name, fd
	net.init(fd)
	op = net.getbyte()
	print "READ(4bytes)", op
	if op == net.REQ_MOVE:
		print "- move"
		fromposval = net.getbyte()
		toposval = net.getbyte()
		print " + ", fromposval, toposval
		# TODO: move validity check
		# TODO: send to all players
		frompos = (fromposval % 9, fromposval / 9)
		topos = (toposval % 9, toposval / 9)
		if ggzboardgame.validatemove("w", frompos, topos):
			ggzboardgame.domove(frompos, topos)
			net.sendbyte(net.MSG_MOVE)
			net.sendbyte(fromposval)
			net.sendbyte(toposval)

			(ret, frompos, topos) = ggzboardgame.aimove()
			print "AI:", ret, frompos, topos
			if ret:
				(x, y) = frompos
				(x2, y2) = topos
				fromposval = y * 9 + x
				toposval = y2 * 9 + x2
				ggzboardgame.domove(frompos, topos)
				net.sendbyte(net.MSG_MOVE)
				net.sendbyte(fromposval)
				net.sendbyte(toposval)

		if ggzboardgame.over():
			winner = 0 # FIXME!
			net.sendbyte(net.MSG_GAMEOVER)
			net.sendbyte(winner)
			ggzdmod.reportStatistics(winner)

	if net.error():
		print "** network error! **"
		sys.exit(-1)

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
	global net
	global ggzboardgame

	print "### launched"

	""" Setup """

	print "### go init ggz"

	initggz()
	net = Server()

	print "### now go loop"

	ggzdmod.connect()
	ggzdmod.mainLoop()

	""" Main loop """

	while 1:
		pass

if __name__ == "__main__":
	main()
