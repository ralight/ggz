#!/usr/bin/env python
# GGZBoard Reversi: Reversi game module for the GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import ggzmod

class NetworkInfo:
	def __init__(self):
		self.playernum = -1
		self.playerturn = -1
		self.state = None
		self.modified = 0
		self.returnvalue = -1

class NetworkBase:
	def __init__(self):
		self.sock = None
		self.gamefd = -1

	def handle_network(self):
		networked = 0
		if self.gamefd != -1:
			networked = 1

		ret = ggzmod.autonetwork(networked, self.gamefd)
		if ret == 1 or ret == 3:
			self.network()
		if ret == 2 or ret == 3:
			ggzmod.dispatch()

	def network(self):
		print "ERROR: PURE VIRTUAL FUNCTION!"

	def handle_server(self, fd):
		ggzmod.setState(ggzmod.STATE_PLAYING)

		self.gamefd = fd
		self.init(net.gamefd)

	def init(self, fd):
		self.sock = socket.fromfd(fd, socket.AF_INET, socket.SOCK_STREAM)

	def getbyte(self):
		opstr = self.sock.recv(4)
		op = ord(opstr[0]) * 256 * 256 * 256
		op += ord(opstr[1]) * 256 * 256
		op += ord(opstr[2]) * 256
		op += ord(opstr[3])
		return op

	def getchar(self):
		opstr = self.sock.recv(1)
		op = ord(opstr[0])
		return op

	def getstring(self):
		length = self.getbyte()
		opstr = self.sock.recv(length)
		return opstr

	def sendbyte(self, byte):
		nbyte = socket.htonl(byte)
		c1 = (nbyte >> 0) & 0xFF
		c2 = (nbyte >> 8) & 0xFF
		c3 = (nbyte >> 16) & 0xFF
		c4 = (nbyte >> 24) & 0xFF
		self.sock.send(chr(c1))
		self.sock.send(chr(c2))
		self.sock.send(chr(c3))
		self.sock.send(chr(c4))

	def sendchar(self, char):
		self.sock.send(chr(char))

	def sendstring(self, str):
		self.sendbyte(len(str))
		self.sock.send(str)

class Network(NetworkBase, NetworkInfo):
	def __init__(self):
		NetworkBase.__init__(self)
		NetworkInfo.__init__(self)
		self.MSG_SEAT = 0
		self.MSG_PLAYERS = 1
		self.MSG_MOVE = 2
		self.MSG_GAMEOVER = 3
		self.REQ_MOVE = 4
		self.MSG_START = 5
		self.MSG_SYNC = 6
		self.REQ_SYNC = 7
		self.REQ_AGAIN = 8

		self.SRV_ERROR = -1
		self.SRV_OK = 0
		self.SRV_JOIN = 1
		self.SRV_LEFT = 2
		self.SRV_QUIT = 3

		self.ERROR_INVALIDMOVE = -1
		self.ERROR_WRONGTURN = -2
		self.ERROR_CANTMOVE = -3

	def network(self):
		print "network!"

		op = self.getbyte()

		if op == self.MSG_PLAYERS:
			print "- players"
		else:
			print "- unknown opcode"

ggzboardnet = Network()

