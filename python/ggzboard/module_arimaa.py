#!/usr/bin/env python
# GGZBoard Arimaa: Arimaa game module for the GGZBoard container
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 8
		self.height = 8
		self.swaptiles = 0
		self.autoscaletiles = 0 # due to images
		self.setonly = 0
		self.intersections = 0
		self.noemptytiles = 0
		self.players = 2
		self.turnplayer = 0

		#self.boardstyle = None
		self.boardhints = None

		self.boardstyle = (None)
		self.boardstyle = resize(self.boardstyle, (self.width, self.height))

		for j in range(self.height):
			for i in range(self.width):
				self.boardstyle[i][j] = "arimaa/cell.png"
		self.boardstyle[2][2] = "arimaa/celltrap.png"
		self.boardstyle[2][5] = "arimaa/celltrap.png"
		self.boardstyle[5][2] = "arimaa/celltrap.png"
		self.boardstyle[5][5] = "arimaa/celltrap.png"

		self.boardwidth = 59
		self.boardheight = 59

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.board[0][0] = ("cat", "gold")
		self.board[0][1] = ("dog", "gold")
		self.board[0][2] = ("horse", "gold")
		self.board[0][3] = ("camel", "gold")
		self.board[0][4] = ("elephant", "gold")
		self.board[0][5] = ("horse", "gold")
		self.board[0][6] = ("dog", "gold")
		self.board[0][7] = ("cat", "gold")
		self.board[1][0] = ("rabbit", "gold")
		self.board[1][1] = ("rabbit", "gold")
		self.board[1][2] = ("rabbit", "gold")
		self.board[1][3] = ("rabbit", "gold")
		self.board[1][4] = ("rabbit", "gold")
		self.board[1][5] = ("rabbit", "gold")
		self.board[1][6] = ("rabbit", "gold")
		self.board[1][7] = ("rabbit", "gold")

		self.board[7][0] = ("cat", "silver")
		self.board[7][1] = ("dog", "silver")
		self.board[7][2] = ("horse", "silver")
		self.board[7][3] = ("camel", "silver")
		self.board[7][4] = ("elephant", "silver")
		self.board[7][5] = ("horse", "silver")
		self.board[7][6] = ("dog", "silver")
		self.board[7][7] = ("cat", "silver")
		self.board[6][0] = ("rabbit", "silver")
		self.board[6][1] = ("rabbit", "silver")
		self.board[6][2] = ("rabbit", "silver")
		self.board[6][3] = ("rabbit", "silver")
		self.board[6][4] = ("rabbit", "silver")
		self.board[6][5] = ("rabbit", "silver")
		self.board[6][6] = ("rabbit", "silver")
		self.board[6][7] = ("rabbit", "silver")

		self.isover = 0

		self.datapath = "./"

		self.dice = None
		self.playercolours = None
		self.winner = None
		self.help = None

	def init(self, path):
		self.datapath = path

	def name(self):
		return _("Arimaa")

	def figure(self, piece):
		(gfx, color) = piece
		colorstr = color
		piecestr = gfx
		return self.datapath + "arimaa/" + piecestr + "_" + colorstr + ".png"

	def validatemove(self, fromcolor, frompos, topos):
		(oldx, oldy) = frompos
		(x, y) = topos

		valid = 1
		reason = ""

		if oldx < 0 or oldx >= self.width or oldy < 0 or oldy >= self.height:
			valid = 0
			reason = "out of bounds"
		elif x < 0 or x >= self.width or y < 0 or y >= self.height:
			valid = 0
			reason = "out of bounds"
		elif self.board[oldy][oldx] is None:
			valid = 0
			reason = "no piece selected"
		else:
			f = self.board[oldy][oldx]
			(gfx, color) = f
			if fromcolor == "":
				fromcolor = color
			if color != fromcolor:
				valid = 0
				reason = "not your piece"
			else:
				pass
				# ???

		if valid == 0:
			print reason
		return valid

	def trymove(self, frompos, topos):
		ret = 0

		(oldx, oldy) = frompos
		(x, y) = topos
		valid = self.validatemove("gold", frompos, topos)

		if valid:
			(figure, color) = self.board[oldy][oldx]
			print "TRYMOVE", frompos, topos
			ret = 1
		return ret

	def aimove(self):
		# FIXME: find???
		self.isover = 1
		return (0, 0, 0)

		frompos = (fromval % 8, fromval / 8)
		topos = (toval % 8, toval / 8)
		if ret == 0:
			self.isover = 1
		else:
			pass
			# FIXME: move
		return (ret, frompos, topos) 

	def domove(self, frompos, topos):
		(oldx, oldy) = frompos
		(x, y) = topos
		self.board[y][x] = self.board[oldy][oldx]
		self.board[oldy][oldx] = None

	def over(self):
		return self.isover

	def toggleplayer(self):
		#pass
		self.turnplayer = (self.turnplayer + 1) % self.players

ggzboardgame = Game()
