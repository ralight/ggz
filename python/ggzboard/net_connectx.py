# GGZBoard ConnectX: ConnectX game module for the GGZBoard container
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import ggzmod
import socket

from ggzboard_net import *

class NetworkInfo:
	def __init__(self):
		self.playernum = -1
		#self.playerturn = -1
		self.playernames = None

		self.min_boardwidth = None
		self.max_boardwidth = None
		self.min_boardheight = None
		self.max_boardheight = None
		self.min_connectlength = None
		self.max_connectlength = None

		self.connectlength = None
		self.boardwidth = None
		self.boardheight = None

		# fixme: should be in superclass?
		self.modified = 0

		# fixme: this one too?
		self.movequeue = []

class Network(NetworkBaseClient, NetworkInfo):
	def __init__(self):
		NetworkBaseClient.__init__(self)
		NetworkInfo.__init__(self)

		self.MSG_SEAT = 0
		self.MSG_PLAYERS = 1
		self.MSG_MOVE = 2
		self.MSG_GAMEOVER = 3
		self.REQ_MOVE = 4
		self.RSP_MOVE = 5
		self.SND_SYNC = 6
		self.MSG_OPTIONS = 7
		self.REQ_OPTIONS = 8
		self.MSG_CHAT = 9
		self.RSP_CHAT = 10

		self.SND_MOVE = 0
		self.REQ_SYNC = 1
		self.SND_OPTIONS = 2
		self.REQ_NEWGAME = 3

		self.ERR_STATE = -1
		self.ERR_TURN = -2
		self.ERR_BOUND = -3
		self.ERR_FULL = -4

	def network(self):
		print "network!"

		op = self.getbyte()

		if op == self.MSG_SEAT:
			print "- seat message"
			myseat = self.getbyte()
			print " + seat", myseat
			self.playernum = myseat
		elif op == self.MSG_PLAYERS:
			print "- players message"
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
			print "- move message"
			column = self.getbyte()
			print " + move", column
			self.movequeue.append(((None, (column, 0))))
		elif op == self.MSG_GAMEOVER:
			print "- gameover message"
			winner = self.getchar()
			print " + winner", winner
			self.inputallowed = 0
		elif op == self.REQ_MOVE:
			print "- move request"
			self.inputallowed = 1
		elif op == self.RSP_MOVE:
			print "- move response"
			status = self.getchar()
			print " + status", status
			column = self.getbyte()
			print " + move", column
			if status == 0:
				self.movequeue.append(((None, (column, 0))))
		elif op == self.SND_SYNC:
			print "- synchronisation"
		elif op == self.MSG_OPTIONS:
			print "- option message"
			self.boardwidth = self.getchar()
			self.boardheight = self.getchar()
			self.connectlength = self.getchar()
			print " + options(board width)", self.boardwidth
			print " + options(board height)", self.boardheight
			print " + options(connect length)", self.connectlength
		elif op == self.REQ_OPTIONS:
			print "- option request"
			self.min_boardwidth = self.getchar()
			self.max_boardwidth = self.getchar()
			self.min_boardheight = self.getchar()
			self.max_boardheight = self.getchar()
			self.min_connectlength = self.getchar()
			self.max_connectlength = self.getchar()
			print " + options(board width)", self.min_boardwidth, self.max_boardwidth
			print " + options(board height)", self.min_boardheight, self.max_boardheight
			print " + options(connect length)", self.min_connectlength, self.max_connectlength
			self.sendoptions()
		elif op == self.MSG_CHAT:
			print "- chat message"
		elif op == self.RSP_CHAT:
			print "- chat response"
		else:
			print "- unknown opcode"
			self.errorcode = 1

	def sendoptions(self):
		self.boardheight = 7
		self.boardwidth = 7
		self.connectlength = 4

		self.sendbyte(self.SND_OPTIONS)
		self.sendchar(self.boardheight)
		self.sendchar(self.boardwidth)
		self.sendchar(self.connectlength)
		print "*** SENT OPTIONS"

	def domove(self, frompos, topos):
		self.sendbyte(self.SND_MOVE)
		(x, y) = topos
		self.sendbyte(x)
		print "*** SENT MOVE", x

	def netmove(self):
		if len(self.movequeue) == 0:
			return None
		else:
			move = self.movequeue.pop(0)
			return move

ggzboardnet = Network()

