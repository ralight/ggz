#!/usr/bin/env python
# GGZBoard Checkers: Checkers game module for the GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 8
		self.height = 8
		self.swaptiles = 1
		self.setonly = 0

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.board[0][1] = ("piece", "w")
		self.board[0][3] = ("piece", "w")
		self.board[0][5] = ("piece", "w")
		self.board[0][7] = ("piece", "w")
		self.board[1][0] = ("piece", "w")
		self.board[1][2] = ("piece", "w")
		self.board[1][4] = ("piece", "w")
		self.board[1][6] = ("piece", "w")
		self.board[2][1] = ("piece", "w")
		self.board[2][3] = ("piece", "w")
		self.board[2][5] = ("piece", "w")
		self.board[2][7] = ("piece", "w")

		self.board[5][0] = ("piece", "b")
		self.board[5][2] = ("piece", "b")
		self.board[5][4] = ("piece", "b")
		self.board[5][6] = ("piece", "b")
		self.board[6][1] = ("piece", "b")
		self.board[6][3] = ("piece", "b")
		self.board[6][5] = ("piece", "b")
		self.board[6][7] = ("piece", "b")
		self.board[7][0] = ("piece", "b")
		self.board[7][2] = ("piece", "b")
		self.board[7][4] = ("piece", "b")
		self.board[7][6] = ("piece", "b")

		self.isover = 0

	def name(self):
		return "Checkers/Draughts"

	def figure(self, piece):
		(gfx, color) = piece
		if color == "w":
			colorstr = "white"
		else:
			colorstr = "black"
		if gfx == "piece":
			piecestr = "piece"
		else:
			piecestr = "king"
		return "checkers/" + piecestr + "-" + colorstr + ".svg"

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
		elif self.board[y][x] is not None:
			valid = 0
			reason = "cannot move to occupied square"
		else:
			f = self.board[oldy][oldx]
			(gfx, color) = f
			if fromcolor == "":
				fromcolor = color
			if color != fromcolor:
				valid = 0
				reason = "not your piece"
			elif not (x + y) % 2:
				valid = 0
				reason = "moves only allowed on dark squares"
			else:
				if gfx == "piece":
					moves = (0, 1)
					jumps = (0, 2)
				elif gfx == "king":
					moves = (-7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7)
					jumps = (-2, 0, 2)
				if fromcolor == "w":
					factor = 1
				else:
					factor = -1
				move = (y - oldy) * factor
				if abs(y - oldy) != abs(x - oldx):
					valid = 0
					reason = "move only allowed diagonally"
				elif move not in moves:
					if move not in jumps:
						valid = 0
						reason = "move too long"
					else:
						f2 = self.board[(oldy + y) / 2][(oldx + x) / 2]
						if f2:
							(gfx2, color2) = f2
							if color2 == fromcolor:
								valid = 0
								reason = "must jump over opponent (black) piece"
						else:
							valid = 0
							reason = "must jump over a piece"
				else:
					xfactor = 1
					yfactor = 1
					if oldy > y:
						yfactor = -1
					if oldx > x:
						xfactor = -1
					for p in range(abs(y - oldy)):
						if p == 0 or p == 1:
							continue
						tmpx = oldx + p * xfactor
						tmpy = oldy + p * yfactor
						print "move-empty-check", p, tmpx, tmpy
						f = self.board[tmpy][tmpx]
						if f:
							valid = 0
							reason = "far jump is not allowed"

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
		ret = 0
		frompos = -1
		topos = -1
		value = 0
		for j in range(8):
			for i in range(8):
				f = self.board[j][i]
				if f:
					(gfx, color) = f
					if color == "b":
						frompostmp = (i, j)
						if value < 2:
							jumpmoves = ((i - 2, j - 2), (i + 2, j - 2))
							if gfx == "king":
								jumpmoves = ((i - 2, j - 2), (i + 2, j - 2), (i - 2, j + 2), (i + 2, j + 2))
							for topostmp in jumpmoves:
								valid = self.validatemove(color, frompostmp, topostmp)
								if valid:
									frompos = frompostmp
									topos = topostmp
									print "AIMOVE(2)", frompos, topos
									ret = 1
									value = 2
						if value < 1:
							movemoves = ((i - 1, j - 1), (i + 1, j - 1))
							for topostmp in movemoves:
								valid = self.validatemove(color, frompostmp, topostmp)
								if valid:
									frompos = frompostmp
									topos = topostmp
									print "AIMOVE(1)", frompos, topos
									ret = 1
									value = 1
		if ret == 0:
			self.isover = 1
		return (ret, frompos, topos) 

	def domove(self, frompos, topos):
		if self.validatemove("", frompos, topos):
			(oldx, oldy) = frompos
			(x, y) = topos
			self.board[y][x] = self.board[oldy][oldx]
			self.board[oldy][oldx] = None

			if y == 0 or y == 7:
				f = self.board[y][x]
				(gfx, color) = f
				gfx = "king"
				self.board[y][x] = (gfx, color)
				print "move-executor: kingification"
			if abs(y - oldy) == 2:
				if self.board[(y + oldy) / 2][(x + oldx) / 2]:
					self.board[(y + oldy) / 2][(x + oldx) / 2] = None
					print "move-executor: remove-jumped-piece"

	def over(self):
		return self.isover

