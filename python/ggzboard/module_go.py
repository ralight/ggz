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

		self.boardstyle = None

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.isover = 0
		self.lastmove = ""

	def name(self):
		return "Go"

	def figure(self, piece):
		(gfx, color) = piece
		if color == "w":
			colorstr = "white"
		else:
			colorstr = "black"
		piecestr = "piece"
		return "go/" + piecestr + "-" + colorstr + ".svg"

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
		if self.validatemove(self.lastmove, topos):
			(x, y) = topos
			self.board[y][x] = ("piece", self.lastmove)

	def over(self):
		return self.isover

	def toggleplayer(self):
		pass

ggzboardgame = Game()

