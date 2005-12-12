#!/usr/bin/env python
# GGZBoard Chess: Chess game module for the GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

import ggzchess

class Game:
	def __init__(self):
		self.width = 8
		self.height = 8
		self.swaptiles = 1
		self.autoscaletiles = 1
		self.setonly = 0
		self.intersections = 0
		self.noemptytiles = 0
		self.players = 2
		self.turnplayer = 0

		self.boardstyle = None
		self.boardhints = None

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.board[0][0] = ("rook", "w")
		self.board[0][1] = ("knight", "w")
		self.board[0][2] = ("bishop", "w")
		self.board[0][3] = ("queen", "w")
		self.board[0][4] = ("king", "w")
		self.board[0][5] = ("bishop", "w")
		self.board[0][6] = ("knight", "w")
		self.board[0][7] = ("rook", "w")
		self.board[1][0] = ("pawn", "w")
		self.board[1][1] = ("pawn", "w")
		self.board[1][2] = ("pawn", "w")
		self.board[1][3] = ("pawn", "w")
		self.board[1][4] = ("pawn", "w")
		self.board[1][5] = ("pawn", "w")
		self.board[1][6] = ("pawn", "w")
		self.board[1][7] = ("pawn", "w")

		self.board[7][0] = ("rook", "b")
		self.board[7][1] = ("knight", "b")
		self.board[7][2] = ("bishop", "b")
		self.board[7][3] = ("queen", "b")
		self.board[7][4] = ("king", "b")
		self.board[7][5] = ("bishop", "b")
		self.board[7][6] = ("knight", "b")
		self.board[7][7] = ("rook", "b")
		self.board[6][0] = ("pawn", "b")
		self.board[6][1] = ("pawn", "b")
		self.board[6][2] = ("pawn", "b")
		self.board[6][3] = ("pawn", "b")
		self.board[6][4] = ("pawn", "b")
		self.board[6][5] = ("pawn", "b")
		self.board[6][6] = ("pawn", "b")
		self.board[6][7] = ("pawn", "b")

		self.isover = 0

		ggzchess.init(ggzchess.WHITE, 2)

		self.datapath = "./"

		self.dice = None
		self.playercolours = None
		self.winner = None
		self.help = None

		self.selection = None

	def init(self, path):
		self.datapath = path

	def name(self):
		return _("Chess")

	def figure(self, piece):
		(gfx, color) = piece
		colorstr = color
		piecestr = gfx
		return self.datapath + "chess/" + piecestr + "_" + colorstr + ".svg"

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
				valid = ggzchess.move(oldy * 8 + oldx, y * 8 + x, 0)
				if not valid:
					reason = "(ggzchess engine said so)"

		if valid == 0:
			print reason
		return valid

	def trymove(self, frompos, topos):
		ret = 0

		(oldx, oldy) = frompos
		(x, y) = topos
		valid = self.validatemove("w", frompos, topos)

		if valid:
			(figure, color) = self.board[oldy][oldx]
			print "TRYMOVE", frompos, topos
			ret = 1
		return ret

	def aimove(self):
		(ret, (fromval, toval)) = ggzchess.find(ggzchess.BLACK)
		frompos = (fromval % 8, fromval / 8)
		topos = (toval % 8, toval / 8)
		if ret == 0:
			self.isover = 1
		else:
			ggzchess.move(fromval, toval, 0)
		return (ret, frompos, topos) 

	def domove(self, frompos, topos):
		#if self.validatemove("", frompos, topos):
		(oldx, oldy) = frompos
		(x, y) = topos
		self.board[y][x] = self.board[oldy][oldx]
		self.board[oldy][oldx] = None

		# XXX: why not validatemove() above?
		#valid = ggzchess.move(oldy * 8 + oldx, y * 8 + x, 0)
		ggzchess.move(oldy * 8 + oldx, y * 8 + x, 0)

		if ggzchess.checkmate():
			self.isover = 1

	def over(self):
		return self.isover

	def toggleplayer(self):
		#pass
		self.turnplayer = (self.turnplayer + 1) % self.players

ggzboardgame = Game()
