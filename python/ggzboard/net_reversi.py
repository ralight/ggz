#!/usr/bin/env python
# GGZBoard Reversi: Reversi game module for the GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import ggzmod
import socket

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
		self.errorcode = 0
		self.inputallowed = 0

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
		print "ERROR: PURE VIRTUAL METHOD!"

	def handle_server(self, fd):
		ggzmod.setState(ggzmod.STATE_PLAYING)

		self.gamefd = fd
		self.init(self.gamefd)

	def init(self, fd):
		self.sock = socket.fromfd(fd, socket.AF_INET, socket.SOCK_STREAM)

	def connect(self):
		ggzmod.setHandler(ggzmod.EVENT_SERVER, self.handle_server)
		ret = ggzmod.connect()
		return ret

	def error(self):
		return self.errorcode

	def allowed(self):
		return self.inputallowed

	def getbyte(self):
		opstr = self.sock.recv(4)
		if len(opstr) < 4:
			self.errorcode = 1
			return 0
		c1 = ord(opstr[0])
		c2 = ord(opstr[1])
		c3 = ord(opstr[2])
		c4 = ord(opstr[3])
		op = c1 * 256 * 256 * 256
		op += c2 * 256 * 256
		op += c3 * 256
		op += c4
		if op >= 2**31:
			op -= 2**32
		return op

	def getchar(self):
		opstr = self.sock.recv(1)
		if len(opstr) < 1:
			self.errorcode = 1
			return 0
		op = ord(opstr[0])
		return op

	def getstring(self):
		length = self.getbyte()
		if not self.errorcode:
			opstr = self.sock.recv(length)
			if len(opstr) < length:
				self.errorcode = 1
				return ""
		return opstr

	def sendbyte(self, byte):
		nbyte = socket.htonl(byte)
		c1 = (nbyte >> 24) & 0xFF
		c2 = (nbyte >> 16) & 0xFF
		c3 = (nbyte >> 8) & 0xFF
		c4 = (nbyte >> 0) & 0xFF
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

		self.movequeue = []

	def network(self):
		print "network!"

		op = self.getbyte()

		if op == self.MSG_SEAT:
			print "- seat"
			myseat = self.getbyte()
			print " + seat", myseat
		elif op == self.MSG_PLAYERS:
			print "- players"
			for i in range(2):
				seat = self.getbyte()
				print " + seat", seat
				if seat != 1: #ggzmod.SEAT_OPEN:
					player = self.getstring()
					print " + player", player
		elif op == self.MSG_MOVE:
			print "- move"
			move = self.getbyte()
			print " + move", move
			# FIXME: error handling...
			if move > 0:
				topos = (move % 8, move / 8)
				self.movequeue.append((None, topos))
		elif op == self.MSG_GAMEOVER:
			print "- gameover"
			winner = self.getbyte()
			print " + winner", winner
#		elif op == self.REQ_MOVE:
#			print "- req move"
		elif op == self.MSG_START:
			print "- start"
			self.inputallowed = 1
		elif op == self.MSG_SYNC:
			print "- sync"
#		elif op == self.REQ_SYNC:
#			print "- req sync"
#		elif op == self.REQ_AGAIN:
#			print "- req again"
		else:
			print "- unknown opcode"
			self.errorcode = 1

	def domove(self, frompos, topos):
		self.sendbyte(self.REQ_MOVE)
		(x, y) = topos
		toposval = y * 8 + x
		self.sendbyte(toposval)
		print "*** SENT", toposval

	def netmove(self):
		if len(self.movequeue) == 0:
			return None
		else:
			move = self.movequeue.pop(0)
			return move

ggzboardnet = Network()

