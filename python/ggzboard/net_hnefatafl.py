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

#		self.state = None
#		self.returnvalue = -1

class Network(NetworkBase, NetworkInfo):
	def __init__(self):
		NetworkBase.__init__(self)
		NetworkInfo.__init__(self)
		self.MSG_SEAT = 0
		self.MSG_PLAYERS = 1
		self.MSG_START = 5

		self.MSG_MOVE = 2
		self.MSG_GAMEOVER = 3
		self.REQ_MOVE = 4

#		self.MSG_SYNC = 6
#		self.REQ_SYNC = 7
#		self.REQ_AGAIN = 8

#		self.SRV_ERROR = -1
#		self.SRV_OK = 0
#		self.SRV_JOIN = 1
#		self.SRV_LEFT = 2
#		self.SRV_QUIT = 3

#		self.ERROR_INVALIDMOVE = -1
#		self.ERROR_WRONGTURN = -2
#		self.ERROR_CANTMOVE = -3

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
				if seat != ggzmod.SEAT_OPEN:
					player = self.getstring()
					print " + player", player
		elif op == self.MSG_MOVE:
			print "- move"
			fromposval = self.getbyte()
			toposval = self.getbyte()
			print " + move", fromposval, toposval
			frompos = (fromposval % 9, fromposval / 9)
			topos = (toposval % 9, toposval / 9)
			self.movequeue.append((frompos, topos))
		elif op == self.MSG_GAMEOVER:
			print "- gameover"
			winner = self.getbyte()
			print " + winner", winner
			self.inputallowed = 0
		elif op == self.MSG_START:
			print "- start"
			self.inputallowed = 1
#		elif op == self.MSG_SYNC:
#			print "- sync"
		else:
			print "- unknown opcode"
			self.errorcode = 1

	def domove(self, frompos, topos):
		self.sendbyte(self.REQ_MOVE)
		(x, y) = frompos
		(x2, y2) = topos
		fromposval = y * 9 + x
		toposval = y2 * 9 + x2
		self.sendbyte(fromposval)
		self.sendbyte(toposval)
		print "*** SENT", fromposval, toposval

	def netmove(self):
		if len(self.movequeue) == 0:
			return None
		else:
			move = self.movequeue.pop(0)
			return move

ggzboardnet = Network()

