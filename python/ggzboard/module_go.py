#!/usr/bin/env python
# GGZBoard Go: Go game module for the GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 19
		self.height = 19
		self.swaptiles = 0
		self.autoscaletiles = 1
		self.setonly = 1
		self.intersections = 1
		self.noemptytiles = 0
		self.players = 2
		self.turnplayer = 0

		self.boardstyle = None
		self.boardhints = None

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.isover = 0
		self.lastmove = ""

		self.datapath = "./"

		self.dice = None
		self.playercolours = None
		self.winner = None
		self.help = None

	def init(self, path):
		self.datapath = path

	def name(self):
		return _("Go")

	def figure(self, piece):
		(gfx, color) = piece
		if color == "w":
			colorstr = "white"
		else:
			colorstr = "black"
		piecestr = "piece"
		return self.datapath + "go/" + piecestr + "-" + colorstr + ".svg"

	def validatemove(self, fromcolor, topos):
		(x, y) = topos

		valid = 1
		reason = ""

		if x < 0 or x >= self.width or y < 0 or y >= self.height:
			valid = 0
			reason = "out of bounds"
		elif self.board[y][x] is not None:
			valid = 0
			reason = "cannot move to occupied square"
		else:
			pass

		if valid == 0:
			print reason
		return valid

	def trymove(self, frompos, topos):
		ret = 0

		(x, y) = topos
		# from is None
		valid = self.validatemove("w", topos)

		if valid:
			print "TRYMOVE", topos
			ret = 1
			self.lastmove = "w"
		return ret

	def aimove(self):
		ret = 0

		if ret == 0:
			self.isover = 1
		else:
			self.lastmove = "b"
		return (ret, None, None)

	def domove(self, frompos, topos):
		if self.lastmove == "":
			self.lastmove = "b"
		if self.validatemove(self.lastmove, topos):
			(x, y) = topos
			self.board[y][x] = ("piece", self.lastmove)
			if self.lastmove == "w":
				self.lastmove = "b"
			else:
				self.lastmove = "w"

			# find captures
			print "===== CAPTURE CHECK BEGIN"
			print "affected", topos, self.lastmove
			for start in ((x, y + 1), (x, y - 1), (x + 1, y), (x - 1, y)):
				(i, j) = start
				if i < 0 or i >= self.width or j < 0 or j >= self.height:
					continue
				piece = self.board[j][i]
				if not piece:
					continue
				else:
					print "--> found potential start piece", start, piece
					(gfx, color) = piece
					if color != self.lastmove:
						continue
				print "examine", start
				moves = []
				moves.append(start)
				liberties = 0
				expand = 1
				while expand:
					expand = 0
					for pos in moves:
						(i, j) = pos
						for p in ((i, j + 1), (i, j - 1), (i + 1, j), (i - 1, j)):
							(k, l) = p
							if k < 0 or k >= self.width or l < 0 or l >= self.height:
								continue
							piece = self.board[l][k]
							if not piece:
								liberties += 1
							else:
								if p not in moves:
									(gfx, color) = piece
									if color == self.lastmove:
										moves.append(p)
										expand += 1
					print "--> capture from", start, moves
					print "--> liberties", liberties, "expanded", expand
				if liberties == 0:
					print "NO LIBERTIES!"
					print "REMOVE", moves
					for pos in moves:
						(i, j) = pos
						self.board[j][i] = None
				else:
					print "not considered", start
			print "===== CAPTURE CHECK END"

	def over(self):
		return self.isover

	def toggleplayer(self):
		pass

ggzboardgame = Game()

