#!/usr/bin/env python
# GGZBoard Hnefatafl: Hnefatafl game module for the GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import ggzmod
import socket

from ggzboard_net import *

class NetworkInfo:
	def __init__(self):
		self.playernum = -1
		self.playerturn = -1
		self.playernames = None
		self.modified = 0
		self.playernum = -1

class Network(NetworkBase, NetworkInfo):
	def __init__(self):
		NetworkBase.__init__(self)
		NetworkInfo.__init__(self)

		self.movequeue = []

		self.sequence = -1
		self.command_ok = 0
		self.command_deny = 1
		self.command_newgame = 2
		self.command_query = 3
		self.command_answer = 4
		self.command_move = 5
		self.command_takeback = 6
		self.command_extended = 7

	def network(self):
		print "network!"
		print "*** NOT IMPLEMENTED! (receive messages)"

	def domove(self, frompos, topos):
		(x, y) = frompos
		(x2, y2) = topos
		print "*** NOT IMPLEMENTED! (send messages)"

		c1 = 0x01 # sequence byte
		c2 = 0x01 # checksum byte
		c3 = 0x01 # data byte 1
		c4 = 0x01 # data byte 2

		line = ""
		line += chr(c1)
		line += chr(c2)
		line += chr(c3)
		line += chr(c4)
		self.sendchars(line, len(line))

	def netmove(self):
		if len(self.movequeue) == 0:
			return None
		else:
			move = self.movequeue.pop(0)
			return move

	def getchars(self, length):
		chars = os.read(self.fd, 256)
		print "go::read", len(chars)
		return (len(chars), chars)

	def sendchars(self, chars, length):
		print "go::write", length
		os.write(self.fd, chars)

ggzboardnet = Network()

