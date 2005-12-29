#!/usr/bin/env python
# Bogaprot: Common protocol implementation for board games (server side)
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import sys
import time

import ggzdmod
from ggzboard_net import *

# vars: "winner" for send_gameover
# vars: "seat" for send_reqmove
# vars: "status", ("message") for send_rspmove
# vars: "source", "destination", ("type") for send_move
# vars: "value" for send_dice
# vars: "source, "destination", ("type") for receive_move

class BogaprotServer(NetworkBase):

    ### Class definition

	def __init__(self):
		NetworkBase.__init__(self)
		self.REQ_HANDSHAKE = 101
		self.RSP_HANDSHAKE = 102
		self.MSG_SEAT = 103
		self.MSG_PLAYERS = 104
		self.MSG_START = 105
		self.REQ_MOVE = 106
		self.RSP_MOVE = 107
		self.MSG_MOVE = 108
		self.MSG_DICE = 109
		self.MSG_GAMEOVER = 110

		self.vars = {}

	def broadcast(self, method):
		print "=> broadcast!"
		for i in range(ggzdmod.getNumSeats()):
			method(i)
		print "<= done."

	### Send methods

	def send_seat(self, i):
		print "-> send seat to", i
		fd = ggzdmod.seatFd(i)
		if fd != -1:
			self.init(fd)
			self.sendbyte(self.MSG_SEAT)
			self.sendbyte(i)
		print "<- done"

	def send_players(self, i):
		print "-> send players to", i
		fd = ggzdmod.seatFd(i)
		if fd != -1:
			self.init(fd)
			self.sendbyte(self.MSG_PLAYERS)
			self.sendbyte(ggzdmod.getNumSeats())
			for j in range(ggzdmod.getNumSeats()):
				type = ggzdmod.seatType(j)
				self.sendbyte(type)
				if type != ggzdmod.SEAT_OPEN:
					name = ggzdmod.seatName(j)
					self.sendstring(name)
		print "<- done"

	def send_start(self, i):
		print "-> send start to", i
		fd = ggzdmod.seatFd(i)
		if fd != -1:
			self.init(fd)
			self.sendbyte(self.MSG_START)
		print "<- done"

	def send_gameover(self, i):
		print "-> send gameover to", i
		fd = ggzdmod.seatFd(i)
		if fd != -1:
			self.init(fd)
			self.sendbyte(self.MSG_GAMEOVER)
			self.sendbyte(self.vars["winner"])
		print "<- done"

	def send_reqmove(self, i):
		print "-> send reqmove to", i
		fd = ggzdmod.seatFd(i)
		if fd != -1:
			self.init(fd)
			self.sendbyte(self.REQ_MOVE)
			self.sendbyte(self.vars["seat"])
		print "<- done"

	def send_rspmove(self, i):
		print "-> send rspmove to", i
		fd = ggzdmod.seatFd(i)
		if fd != -1:
			self.init(fd)
			self.sendbyte(self.RSP_MOVE)
			self.sendbyte(self.vars["status"])
			if self.vars["status"] != 0:
				self.sendstring(self.vars["message"])
		print "<- done"

	def send_move(self, i):
		print "-> send move to", i
		fd = ggzdmod.seatFd(i)
		if fd != -1:
			self.init(fd)
			self.sendbyte(self.MSG_MOVE)
			self.sendbyte(self.vars["source"])
			self.sendbyte(self.vars["destination"])
			if self.vars["source"] < 0:
				self.sendstring(self.vars["type"])
		print "<- done"

	def send_dice(self, i):
		print "-> send dice to", i
		fd = ggzdmod.seatFd(i)
		if fd != -1:
			self.init(fd)
			self.sendbyte(self.MSG_DICE)
			self.sendbyte(self.vars["value"])
		print "<- done"

	### Receive methods

	def receive_move(self, i):
		print "<- receive move from", i
		fd = ggzdmod.seatFd(i)
		if fd != -1:
			self.init(fd)
			self.vars["source"] = self.getbyte()
			self.vars["destination"] = self.getbyte()
			if self.vars["source"] < 0:
				self.vars["type"] = self.getstring()
		print "-> done"

	### Hooks

	def hook_state(self, state):
		print "* state:", str(state)

	def hook_join(self, num, type, name, fd):
		print "* join:", num, type, name, fd

	def hook_leave(self, num, type, name, fd):
		print "* leave:", num, type, name, fd

	def hook_log(self, line):
		print "* log:", line

	def hook_data(self, num, type, name, fd):
		print "* data:", num, type, name, fd

	def hook_error(self, arg):
		print "* error:", arg
		sys.exit(-1)

	### Initialisation calls

	def initggz(self):
		ggzdmod.setHandler(ggzdmod.EVENT_STATE, self.hook_state)
		ggzdmod.setHandler(ggzdmod.EVENT_JOIN, self.hook_join)
		ggzdmod.setHandler(ggzdmod.EVENT_LEAVE, self.hook_leave)
		ggzdmod.setHandler(ggzdmod.EVENT_LOG, self.hook_log)
		ggzdmod.setHandler(ggzdmod.EVENT_DATA, self.hook_data)
		ggzdmod.setHandler(ggzdmod.EVENT_ERROR, self.hook_error)

	def loop(self):
		print "### launched"
		print "### go init ggz"

		self.initggz()

		print "### now go loop"

		ggzdmod.connect()
		ggzdmod.mainLoop()

		while 1:
			pass

