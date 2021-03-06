# GGZBoard Reversi: Reversi game module for the GGZBoard container
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
			self.playernum = myseat
		elif op == self.MSG_PLAYERS:
			print "- players"
			self.playernames = []
			for i in range(2):
				seat = self.getbyte()
				print " + seat", seat
				player = ""
				if seat != ggzmod.SEAT_OPEN:
					player = self.getstring()
					print " + player", player
				self.playernames.append(player)
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
			self.inputallowed = 0
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

