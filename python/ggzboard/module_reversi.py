#!/usr/bin/env python
# GGZBoard Reversi: Reversi game module for the GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 16
		self.height = 16
		self.swaptiles = 0
		self.setonly = 1

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.board[7][7] = ("piece", "w")
		self.board[7][8] = ("piece", "b")
		self.board[8][7] = ("piece", "b")
		self.board[8][8] = ("piece", "w")

		self.isover = 0

	def name(self):
		return "Reversi"

	def figure(self, piece):
		(gfx, color) = piece
		if color == "w":
			colorstr = "white"
		else:
			colorstr = "black"
		piecestr = "piece"
		return "reversi/" + piecestr + "-" + colorstr + ".svg"

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
		return ret

	def aimove(self):
		ret = 0
		frompos = -1
		topos = -1
		value = 0
		for j in range(self.height):
			for i in range(self.width):
				f = self.board[j][i]
				if not f:
					if value < 1:
						frompos = None
						topos = (i, j)
						print "AIMOVE(1)", frompos, topos
						ret = 1
						value = 1
		if ret == 0:
			self.isover = 1
		return (ret, frompos, topos)

	def domove(self, frompos, topos):
		if self.validatemove("", topos):
			(x, y) = topos
			self.board[y][x] = ("piece", "w")

	def over(self):
		return self.isover

