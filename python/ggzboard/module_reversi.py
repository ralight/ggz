#!/usr/bin/env python
# GGZBoard Reversi: Reversi game module for the GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 8
		self.height = 8
		self.swaptiles = 0
		self.autoscaletiles = 1
		self.setonly = 1
		self.intersections = 0
		self.noemptytiles = 0
		self.players = 2
		self.turnplayer = 0

		self.boardstyle = None
		self.boardhints = None

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.board[3][3] = ("piece", "b")
		self.board[3][4] = ("piece", "w")
		self.board[4][3] = ("piece", "w")
		self.board[4][4] = ("piece", "b")

		self.isover = 0
		self.lastmove = ""

		self.datapath = "./"

		self.dice = None
		self.playercolours = None
		self.winner = None
		self.help = None

		self.selection = None

	# !!!
	def selectpiece(self, placepos):
		return

	def init(self, path):
		self.datapath = path

	def name(self):
		return _("Reversi")

	def figure(self, piece):
		(gfx, color) = piece
		if color == "w":
			colorstr = "white"
		else:
			colorstr = "black"
		piecestr = "piece"
		return self.datapath + "reversi/" + piecestr + "-" + colorstr + ".svg"

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
			valid = 0
			for i in range(x - 1, x + 2):
				for j in range(y - 1, y + 2):
					if i >= 0 and i < self.width and j >= 0 and j < self.height:
						if self.board[j][i]:
							valid = 1
			if valid == 0:
				reason = "has to border an existing piece"

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
#			self.lastmove = "w"
		return ret

	def aimove(self):
		ret = 0
		topos = -1
		value = 0
		for j in range(self.height):
			for i in range(self.width):
				f = self.board[j][i]
				if not f:
					if value < 1:
						topos = (i, j)
						if self.validatemove("b", topos):
							print "AIMOVE(1)", topos
							ret = 1
							value = 1
		if ret == 0:
			self.isover = 1
#		else:
#			self.lastmove = "b"
		return (ret, None, topos)

	def domove(self, frompos, topos):
		if self.validatemove(self.lastmove, topos):
			(x, y) = topos
			self.board[y][x] = ("piece", self.lastmove)

			maxdist = max(self.width, self.height)
			xmin = -maxdist
			xmax = maxdist
			ymin = -maxdist
			ymax = maxdist
			list = []
			print "LIMIT", xmin, xmax, ymin, ymax
			for pos in ((xmin, 0), (xmax, 0), (0, ymin), (0, ymax), (xmin, ymin), (xmin, ymax), (xmax, ymin), (xmax, ymax)):
				(px, py) = pos
				i = x
				j = y
				finished = 0
				tmplist = []
				print "limit-stuff", x, y, "until", x + px, y + py
				while i != x + px or j != y + py:
					if finished:
						break
					if px:
						i = i + (px / abs(px))
					if py:
						j = j + (py / abs(py))
					print "TRY", i, j
					if i >= 0 and i < self.width and j >= 0 and j < self.height:
						if not self.board[j][i]:
							finished = 1
						else:
							(gfx, color) = self.board[j][i]
							if color == self.lastmove:
								for pos in tmplist:
									list.append(pos)
								finished = 1
							else:
								tmplist.append((i, j))
								print "LIST", list

			for pos in list:
				(i, j) = pos
				(gfx, color) = self.board[j][i]
				if color == "w":
					color = "b"
				else:
					color = "w"
				self.board[j][i] = (gfx, color)

	def over(self):
		return self.isover

	def toggleplayer(self):
		if self.lastmove == "w":
			self.lastmove = "b"
		else:
			self.lastmove = "w"

ggzboardgame = Game()

