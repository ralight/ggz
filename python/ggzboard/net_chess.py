# GGZBoard Chess: Chess game module for the GGZBoard container
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

#		self.state = None
#		self.returnvalue = -1

class Network(NetworkBaseClient, NetworkInfo):
	def __init__(self):
		NetworkBaseClient.__init__(self)
		NetworkInfo.__init__(self)

		self.PROTOCOL_VERSION = 6

		self.MSG_SEAT = 1
		self.MSG_PLAYERS = 2
		self.REQ_TIME = 3
		self.RSP_TIME = 4
		self.MSG_START = 5
		self.REQ_MOVE = 6
		self.MSG_MOVE = 7
		self.MSG_GAMEOVER = 8
		self.REQ_UPDATE = 9
		self.RSP_UPDATE = 10
		self.MSG_UPDATE = 11
		self.REQ_FLAG = 12
		self.REQ_DRAW = 13

		self.CLOCK_NOCLOCK = 0
		self.CLOCK_CLIENT = 1
		self.CLOCK_SERVERLAG = 2
		self.CLOCK_SERVER = 3

		self.GAMEOVER_DRAW_AGREEMENT = 1
		self.GAMEOVER_DRAW_STATEMATE = 2
		self.GAMEOVER_DRAW_POSREP = 3
		self.GAMEOVER_DRAW_MATERIAL = 4
		self.GAMEOVER_DRAW_MOVECOUNT = 5
		self.GAMEOVER_DRAW_TIMEMATERIAL = 6
		self.GAMEOVER_WIN_1_MATE = 7
		self.GAMEOVER_WIN_1_RESIGN = 8
		self.GAMEOVER_WIN_1_FLAG = 9
		self.GAMEOVER_WIN_2_MATE = 10
		self.GAMEOVER_WIN_2_RESIGN = 11
		self.GAMEOVER_WIN_2_FLAG = 12

		self.movequeue = []

	def network(self):
		print "network!"

		op = self.getchar()

		if op == self.MSG_SEAT:
			print "- seat"
			seat = self.getchar()
			version = self.getchar()
			print " + seat", seat
			print " + version", version
			self.playernum = seat
			self.playerturn = seat
		elif op == self.MSG_PLAYERS:
			print "- players"
			name1 = "(unnamed)"
			name2 = "(unnamed)"
			type1 = self.getchar()
			if type1 != ggzmod.SEAT_OPEN:
				name1 = self.getstring()
			type2 = self.getchar()
			if type2 != ggzmod.SEAT_OPEN:
				name2 = self.getstring()
			print " + player1", type1, name1
			print " + player2", type2, name2
			self.playernames = []
			self.playernames.append(name1)
			self.playernames.append(name2)
			self.modified = 1
		elif op == self.REQ_TIME:
			print "- reqtime"
			###
			self.sendchar(self.RSP_TIME)
			self.sendbyte(0)
			# 0: no clock is used
			###
		elif op == self.RSP_TIME:
			print "- rsptime"
			time = self.getbyte()
			print " + time", time
		elif op == self.MSG_START:
			print "- start"
			self.inputallowed = 1
		elif op == self.MSG_MOVE:
			print "- msgmove"
			length = self.getbyte()
			print " + length", length
			if length:
				x = self.getchar()
				y = self.getchar()
				x2 = self.getchar()
				y2 = self.getchar()
				cval = self.getchar()
				print " + move", x, y, x2, y2, cval
				frompos = (x - ord('A'), y - ord('1'))
				topos = (x2 - ord('A'), y2 - ord('1'))
				self.movequeue.append((frompos, topos))
		elif op == self.MSG_GAMEOVER:
			print "- gameover"
			cval = self.getbyte()
			print " + cval", cval
			self.inputallowed = 0
		elif op == self.REQ_DRAW:
			print "- reqdraw"
		elif op == self.RSP_UPDATE:
			print "- rspupdate"
			value = self.getbyte()
			value2 = self.getbyte()
			print " + value", value
			print " + value2", value2
		else:
			print "- unknown opcode"
			self.errorcode = 1

	def domove(self, frompos, topos):
		self.sendchar(self.REQ_MOVE)
		(x, y) = frompos
		(x2, y2) = topos
		print "*** SENT", x + ord('A'), y + ord('1'), x2 + ord('A'), y2 + ord('1'), "(", frompos, topos, ")"
		#y = 8 - y
		#y2 = 8 - y2
		#x = 8 - x
		#x2 = 8 - x2
		self.sendchar(0)
		self.sendchar(0)
		self.sendchar(0)
		self.sendchar(6)
		self.sendchar(x + ord('A'))
		self.sendchar(y + ord('1'))
		self.sendchar(x2 + ord('A'))
		self.sendchar(y2 + ord('1'))
		self.sendchar(0)
		self.sendchar(0)

	def netmove(self):
		if len(self.movequeue) == 0:
			return None
		else:
			move = self.movequeue.pop(0)
			return move

ggzboardnet = Network()

