# Bogaprot: Common protocol implementation for board games (client side)
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import sys
import time

import ggzmod
from ggzboard_net import *

# vars: "winner" for send_gameover
# vars: "seat" for send_reqmove
# vars: "status", ("message") for send_rspmove
# vars: "source", "destination", ("type") for send_move
# vars: "value" for send_dice
# vars: "source, "destination", ("type") for receive_move

class BogaprotClient(NetworkBase):

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

	### Receive methods

	def receive_seat(self):
		print "-> receive seat"
		self.vars["seat"] = self.getbyte()
		print "<- done"

	def receive_players(self):
		print "-> receive players"
		numseats = self.getbyte()
		players = []
		for j in range(numseats):
			type = self.getbyte()
			if type != ggzmod.SEAT_OPEN:
				name = self.getstring()
			else:
				name = None
			players.append((type, name))
		self.vars["players"] = players
		print "<- done"

	def receive_start(self):
		print "-> receive start"
		print "<- done"

	def receive_gameover(self):
		print "-> recieve gameover"
		self.vars["winner"] = self.getbyte()
		print "<- done"

	def receive_reqmove(self):
		print "-> receive reqmove"
		self.vars["seat"] = self.getbyte()
		print "<- done"

	def receive_rspmove(self):
		print "-> receive rspmove"
		self.vars["status"] = self.getbyte()
		if self.vars["status"] != 0:
			self.vars["message"] = self.getstring()
		print "<- done"

	def receive_dice(self):
		print "-> receive dice"
		self.vars["value"] = self.getbyte()
		print "<- done"

	def receive_move(self):
		print "-> receive move"
		self.vars["source"] = self.getbyte()
		self.vars["destination"] = self.getbyte()
		if self.vars["source"] < 0:
			self.vars["type"] = self.getstring()
		print "<- done"

	### Send methods

	def send_move(self):
		print "<- send move"
		self.sendbyte(self.MSG_MOVE)
		self.sendbyte(self.vars["source"])
		self.sendbyte(self.vars["destination"])
		if self.vars["source"] < 0:
			self.sendstring(self.vars["type"])
		print "-> done"

