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
import module_hnefatafl
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
		self.MSG_SYNC = 6
		self.REQ_SYNC = 7
		self.REQ_AGAIN = 8

		self.SRV_ERROR = -1
		self.SRV_OK = 0
		self.SRV_JOIN = 1
		self.SRV_LEFT = 2
		self.SRV_QUIT = 3

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
		move = net.getbyte()
		print " + ", move

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
#	global xadrez

	print "### launched"

	""" Setup """

#	xadrez = Xadrez()

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

