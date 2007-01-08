#!/usr/bin/env python
# Server for the Ludo game
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import sys
import time
import random
import os, pwd
import re

import gettext
gettext.install("ggzpython", None, 1)

try:
	import ggzsettings
	DATAPATH = ggzsettings.DATAPATH + "/ggzboard/"
	MODULEPATH = ggzsettings.MODULEPATH + "/ggzboard/"
	I18NPATH = ggzsettings.I18NPATH
	sys.path = [ggzsettings.MODULEPATH + "/ggzboard/"] + sys.path
	sys.path = [ggzsettings.MODULEPATH + "/common/"] + sys.path
except:
	DATAPATH = "./"
	MODULEPATH = "./"
	I18NPATH = "./"
	sys.path = ["../lib/"] + sys.path

import bogaprot
from module_madn import *

class LudoServer(bogaprot.BogaprotServer):
	def __init__(self):
		bogaprot.BogaprotServer.__init__(self)

	def table_full(self):
		full = 1
		for i in range(bogaprot.ggzdmod.getNumSeats()):
			seat = bogaprot.ggzdmod.getSeat(i)
			(number, type, name, fd) = seat
			if type != bogaprot.ggzdmod.SEAT_PLAYER and type != bogaprot.ggzdmod.SEAT_BOT:
				full = 0
		return full

	def hook_join(self, num, type, name, fd):
		print "(ludo) * join:", num, type, name, fd
		self.send_seat(num)
		self.send_players(num)
		if self.table_full():
			self.broadcast(self.send_start)
			self.send_start(0)
			self.vars["seat"] = 0
			self.broadcast(self.send_reqmove)

	def hook_data(self, num, type, name, fd):
		print "(ludo) * data:", num, type, name, fd
		self.init(fd)
		op = self.getbyte()
		print "opcode", op
		if op == self.MSG_MOVE:
			print "- move"
			self.receive_move(num)
			fromposval = self.vars["source"]
			toposval = self.vars["destination"]
			print " + ", fromposval, toposval
			type = None
			if fromposval < 0:
				type = self.vars["type"]
				print " + ", type

			# TODO: move validity check
			# TODO: send to all players
			# FIXME: board size
			frompos = (fromposval % 10, fromposval / 10)
			topos = (toposval % 10, toposval / 10)

			self.vars["status"] = 0
			self.vars["message"] = ""
			self.send_rspmove(num)

			# FIXME: validation
			#if self.game.validatemove("w", frompos, topos):
			if 1 == 1:
				self.game.domove(frompos, topos)
				self.broadcast(self.send_move)
				self.vars["source"] = fromposval
				self.vars["destination"] = toposval
				if fromposval < 0:
					self.vars["type"] = type

				(ret, frompos, topos) = self.game.aimove()
				print "AI:", ret, frompos, topos
				if ret:
					(x, y) = frompos
					(x2, y2) = topos
					fromposval = y * 10 + x
					toposval = y2 * 10 + x2
					self.game.domove(frompos, topos)

					self.vars["seat"] = 1
					self.broadcast(self.send_reqmove)

					self.vars["source"] = fromposval
					self.vars["destination"] = toposval
					self.vars["type"] = 0
					self.broadcast(self.send_move)

			if self.game.over():
				self.vars["winner"] = self.game.winner
				self.broadcast(self.send_gameover)
				bogaprot.ggzdmod.reportStatistics(self.vars["winner"])
			else:
				self.vars["seat"] = 0
				self.broadcast(self.send_reqmove)

		if self.error():
			print "** network error! **"
			sys.exit(-1)

if __name__ == "__main__":
	print "### Ludo (via Bogaprot) ###"

	srv = LudoServer()
	srv.game = ggzboardgame
	srv.loop()

