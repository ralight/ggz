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

		self.boardstyle = (None)
		self.boardstyle = resize(self.boardstyle, (self.width, self.height))

		self.boardstyle[4][4] = (50, 0, 50)

		self.boardstyle[0][0] = (50, 50, 0)
		self.boardstyle[0][8] = (50, 50, 0)
		self.boardstyle[8][0] = (50, 50, 0)
		self.boardstyle[8][8] = (50, 50, 0)

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
			figure = self.board[oldy][oldx]
			(gfx, color) = figure

			if (oldx == x and oldy != y) or (oldx != x and oldy == y):
				if abs(oldx - x) > 1:
					valid = 0
				if abs(oldy - y) > 1:
					valid = 0
				if x == 4 and y == 4:
					if gfx == "piece":
						valid = 0
				if abs(oldx - x) == 2 or abs(oldy - y) == 2:
					if (oldx + x) / 2 == 4 and (oldy + y) / 2 == 4:
						if gfx == "piece":
							print "JUMP OVER THE THRONE!"
							valid = 1
				if color != fromcolor:
					valid = 0
					print "color", color, fromcolor
			else:
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
			ret = 1
			self.lastmove = "w"
		return ret

	def aimove(self):
		ret = 0
		frompos = -1
		topos = -1
		value = 0
		for j in range(self.height):
			if value > 0:
				break
			for i in range(self.width):
				if value > 0:
					break
				f = self.board[j][i]
				if f:
					frompos = (i, j)
					for pos in (-2, -1, 1, 2):
						for dpos in ((pos, 0), (0, pos)):
							if value > 0:
								break
							(di, dj) = dpos
							topos = (i + di, j + dj)
							if self.validatemove("b", frompos, topos):
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

			figure = self.board[y][x]
			(gfx, color) = figure

			if (x == 0 or x == 8) and (y == 0 or y == 8):
				if gfx == "king":
					self.isover = 1

			vc = color
			for pos in ((x + 2, y), (x - 2, y), (x, y - 2), (x, y + 2)):
					(x2, y2) = pos
					x3 = (x2 + x) / 2
					y3 = (y2 + y) / 2
					if x3 >= 0 and x3 < self.width and y3 >= 0 and y3 < self.height:
						figure3 = self.board[y3][x3]
					else:
						figure3 = None
					inside = 0
					if figure3:
						(gfx3, color3) = figure3
						if x2 >= 0 and x2 < self.width and y2 >= 0 and y2 < self.height:
							inside = 1
						if x2 == -1 or y2 == -1 or x2 == self.width or y2 == self.height:
							inside = 2
					if inside:
						corner = 0
						if inside == 1:
							figure2 = self.board[y2][x2]
						elif inside == 2:
							figure2 = None
							if gfx3 == "king":
								print "KING: caught by edge?"
								corner = 1
						if (x2 == 0 or x2 == 8) and (y2 == 0 or y2 == 8):
							print "KING/ZABEL: caught by corner?"
							corner = 1
						if (x2 == 4 and y2 == 4):
							if gfx3 == "king":
								print "KING: caught by throne?"
								corner = 1
						if figure2 or corner:
							if figure2:
								(gfx2, color2) = figure2
							else:
								color2 = vc
							if color2 == vc:
								print "CASE:", x, y, x2, y2
								if color3 != vc:
									print "ERASE HIM :)"
									if gfx3 == "king":
										print "KING :("
										x4 = x3 + (y2 - y) / 2
										y4 = y3 + (x2 - x) / 2
										x5 = x3 - (y2 - y) / 2
										y5 = y3 - (x2 - x) / 2
										inside4 = 0
										inside5 = 0
										if x4 >= 0 and x4 < self.width and y4 >= 0 and y4 < self.height:
											figure4 = self.board[y4][x4]
											inside4 = 1
										else:
											figure4 = None
											inside4 = 2
										if x5 >= 0 and x5 < self.width and y5 >= 0 and y5 < self.height:
											figure5 = self.board[y5][x5]
											inside5 = 1
										else:
											figure5 = None
											inside5 = 2
										corner4 = 0
										corner5 = 0
										if figure4:
											(gfx4, color4) = figure4
											if color4 == vc:
												corner4 = 1
										else:
											if inside4 == 2:
												corner4 = 1
												print "KING(4): caught by edge?"
											else:
												if x4 == 4 and y4 == 4:
													corner4 = 1
													print "KING(4): caught by corner?"
												elif (x4 == 0 or x4 == 8) and (y4 == 0 or y4 == 8):
													corner4 = 1
													print "KING(4): caught by throne?"
										if figure5:
											(gfx5, color5) = figure5
											if color5 == vc:
												corner5 = 1
										else:
											if inside5 == 2:
												corner5 = 1
												print "KING(5): caught by edge?"
											else:
												if x5 == 4 and y5 == 4:
													corner5 = 1
													print "KING(5): caught by corner?"
												elif (x5 == 0 or x5 == 8) and (y5 == 0 or y5 == 8):
													corner5 = 1
													print "KING(5): caught by throne?"
										if corner4 and corner5:
											print "BUT NOW HE's DEAD"
											self.board[y3][x3] = None
											self.isover = 1
									else:
										self.board[y3][x3] = None

	def over(self):
		return self.isover

	def toggleplayer(self):
		pass

ggzboardgame = Game()
