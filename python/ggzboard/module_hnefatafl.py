#!/usr/bin/env python
# GGZBoard Hnefatafl: Hnefatafl game module for the GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 9
		self.height = 9
		self.swaptiles = 0
		self.autoscaletiles = 0
		self.setonly = 0
		self.intersections = 0

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.board[4][4] = ("king", "w")

		self.board[4][2] = ("piece", "w")
		self.board[4][3] = ("piece", "w")
		self.board[4][5] = ("piece", "w")
		self.board[4][6] = ("piece", "w")
		self.board[2][4] = ("piece", "w")
		self.board[3][4] = ("piece", "w")
		self.board[5][4] = ("piece", "w")
		self.board[6][4] = ("piece", "w")

		self.board[0][3] = ("piece", "b")
		self.board[0][4] = ("piece", "b")
		self.board[0][5] = ("piece", "b")
		self.board[1][4] = ("piece", "b")
		self.board[8][3] = ("piece", "b")
		self.board[8][4] = ("piece", "b")
		self.board[8][5] = ("piece", "b")
		self.board[7][4] = ("piece", "b")
		self.board[3][0] = ("piece", "b")
		self.board[4][0] = ("piece", "b")
		self.board[5][0] = ("piece", "b")
		self.board[4][1] = ("piece", "b")
		self.board[3][8] = ("piece", "b")
		self.board[4][8] = ("piece", "b")
		self.board[5][8] = ("piece", "b")
		self.board[4][7] = ("piece", "b")

		self.isover = 0
		self.lastmove = ""

	def name(self):
		return "Hnefatafl"

	def figure(self, piece):
		(gfx, color) = piece
		if color == "w":
			colorstr = "white"
		else:
			colorstr = "black"
		if gfx == "king":
			piecestr = "king"
		else:
			piecestr = "piece"
		return "hnefatafl/" + piecestr + "-" + colorstr + ".svg"

	def validatemove(self, fromcolor, frompos, topos):
		(x, y) = topos
		(oldx, oldy) = frompos

		valid = 1
		reason = ""

		if x < 0 or x >= self.width or y < 0 or y >= self.height:
			valid = 0
			reason = "out of bounds"
		elif oldx < 0 or oldx >= self.width or oldy < 0 or oldy >= self.height:
			valid = 0
			reason = "out of bounds"
		elif self.board[oldy][oldx] is None:
			valid = 0
			reason = "nothing to move"
		elif self.board[y][x] is not None:
			valid = 0
			reason = "cannot move to occupied square"
		else:
			valid = 1
			if oldx == x or oldy == y:
				i = oldx
				j = oldy
				while i != x or j != y:
					if x - oldx:
						i += (x - oldx) / abs(x - oldx)
					if y - oldy:
						j += (y - oldy) / abs(y - oldy)
					if self.board[j][i]:
						valid = 0
			if valid == 0:
				reason = "movement must adhere to rules (rook-like)"

		if valid == 0:
			print reason
		return valid

	def trymove(self, frompos, topos):
		ret = 0

		valid = self.validatemove("w", frompos, topos)

		if valid:
			print "TRYMOVE", frompos, topos
			ret = 1
			self.lastmove = "w"
		return ret

	def aimove(self):
		ret = 0
		frompos = -1
		topos = -1
		value = 0
		for j in range(self.height):
			for i in range(self.width):
				f = self.board[j][i]
				if f:
					if value < 1:
						frompos = (i, j)
						for pos in (-8, 8, -7, 7, -6, 6, -5, 5, -4, 4, -3, 3, -2, 2, -1, 1):
							topos = (i + pos, j)
							if self.validatemove("b", frompos, topos):
								print "AIMOVE(1,h)", frompos, topos
								ret = 1
								value = 1
							topos = (i, j + pos)
							if self.validatemove("b", frompos, topos):
								print "AIMOVE(1,v)", frompos, topos
								ret = 1
								value = 1
		if ret == 0:
			self.isover = 1
		else:
			self.lastmove = "b"
		return (ret, frompos, topos)

	def domove(self, frompos, topos):
		if self.validatemove(self.lastmove, frompos, topos):
			(oldx, oldy) = frompos
			(x, y) = topos
			self.board[y][x] = self.board[oldy][oldx]
			self.board[oldy][oldx] = None

	def over(self):
		return self.isover

ggzboardgame = Game()
