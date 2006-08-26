# GGZBoard Ludo: Ludo game module for the GGZBoard container
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import bogaprotclient

class NetworkInfo:
	def __init__(self):
		self.playernum = -1
		self.playerturn = -1
		self.playernames = None
		self.modified = 0

class Network(bogaprotclient.BogaprotClient, NetworkInfo):
	def __init__(self):
		bogaprotclient.BogaprotClient.__init__(self)
		NetworkInfo.__init__(self)

		self.movequeue = []

	def network(self):
		print "network!"

		op = self.getbyte()

		if op == self.MSG_SEAT:
			print "- seat"
			self.receive_seat()
			self.playernum = self.vars["seat"]
		elif op == self.MSG_PLAYERS:
			print "- players"
			self.receive_players()
			players = self.vars["players"]
			self.playernames = []
			for p in players:
				(type, name) = p
				self.playernames.append(name)
		elif op == self.MSG_START:
			print "- start"
			self.receive_start()
			self.inputallowed = 1
		elif op == self.REQ_MOVE:
			print "- reqmove"
			self.receive_reqmove()
			self.playerturn = self.vars["seat"]
		elif op == self.RSP_MOVE:
			print "- rspmove"
			self.receive_rspmove()
			success = self.vars["status"]
			print "Success?", success
		elif op == self.MSG_MOVE:
			print "- move"
			self.receive_move()
			fromposval = self.vars["source"]
			toposval = self.vars["destination"]
			frompos = (fromposval % 10, fromposval / 10)
			topos = (toposval % 10, toposval / 10)
			self.movequeue.append((frompos, topos))
		elif op == self.MSG_DICE:
			print "- dice"
			self.receive_dice()
		elif op == self.MSG_GAMEOVER:
			print "- gameover"
			self.receive_gameover()
			winner = self.vars["winner"]
			print "WINNER", winner
			self.inputallowed = 0
		else:
			print "- unknown opcode"
			self.errorcode = 1

	def domove(self, frompos, topos):
		(x, y) = frompos
		(x2, y2) = topos
		fromposval = y * 10 + x
		toposval = y2 * 10 + x2
		self.vars["source"] = fromposval
		self.vars["destination"] = toposval
		self.vars["type"] = None
		self.send_move()
		print "*** SENT", fromposval, toposval

	def netmove(self):
		if len(self.movequeue) == 0:
			return None
		else:
			move = self.movequeue.pop(0)
			return move

ggzboardnet = Network()

