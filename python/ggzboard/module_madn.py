#!/usr/bin/env python
# -*- coding: utf-8 -*-
# GGZBoard M-Ä-D-N: Mensch-ärgere-dich-nicht game module for the GGZBoard container
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 11
		self.height = 11
		self.swaptiles = 0
		self.autoscaletiles = 1
		self.setonly = 0
		self.intersections = 0
		self.noemptytiles = 1

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.board[0][0] = ("poeppel", "y")
		self.board[0][1] = ("poeppel", "y")
		self.board[1][0] = ("poeppel", "y")
		self.board[1][1] = ("poeppel", "y")

		self.board[9][9] = ("poeppel", "r")
		self.board[9][10] = ("poeppel", "r")
		self.board[10][9] = ("poeppel", "r")
		self.board[10][10] = ("poeppel", "r")

		self.board[0][9] = ("poeppel", "b")
		self.board[0][10] = ("poeppel", "b")
		self.board[1][9] = ("poeppel", "b")
		self.board[1][10] = ("poeppel", "b")

		self.board[9][0] = ("poeppel", "g")
		self.board[9][1] = ("poeppel", "g")
		self.board[10][0] = ("poeppel", "g")
		self.board[10][1] = ("poeppel", "g")

		self.boardstyle = (None)
		self.boardstyle = resize(self.boardstyle, (self.width, self.height))

		self.boardstyle[0][0] = (150, 150, 0)
		self.boardstyle[0][1] = (150, 150, 0)
		self.boardstyle[1][0] = (150, 150, 0)
		self.boardstyle[1][1] = (150, 150, 0)

		self.boardstyle[9][9] = (150, 0, 0)
		self.boardstyle[9][10] = (150, 0, 0)
		self.boardstyle[10][9] = (150, 0, 0)
		self.boardstyle[10][10] = (150, 0, 0)

		self.boardstyle[0][9] = (0, 0, 200)
		self.boardstyle[0][10] = (0, 0, 200)
		self.boardstyle[1][9] = (0, 0, 200)
		self.boardstyle[1][10] = (0, 0, 200)

		self.boardstyle[9][0] = (0, 150, 0)
		self.boardstyle[9][1] = (0, 150, 0)
		self.boardstyle[10][0] = (0, 150, 0)
		self.boardstyle[10][1] = (0, 150, 0)

		self.boardstyle[0][6] = (0, 0, 200)
		self.boardstyle[1][5] = (0, 0, 200)
		self.boardstyle[2][5] = (0, 0, 200)
		self.boardstyle[3][5] = (0, 0, 200)
		self.boardstyle[4][5] = (0, 0, 200)

		self.boardstyle[10][4] = (0, 150, 0)
		self.boardstyle[9][5] = (0, 150, 0)
		self.boardstyle[8][5] = (0, 150, 0)
		self.boardstyle[7][5] = (0, 150, 0)
		self.boardstyle[6][5] = (0, 150, 0)

		self.boardstyle[4][0] = (150, 150, 0)
		self.boardstyle[5][1] = (150, 150, 0)
		self.boardstyle[5][2] = (150, 150, 0)
		self.boardstyle[5][3] = (150, 150, 0)
		self.boardstyle[5][4] = (150, 150, 0)

		self.boardstyle[6][10] = (150, 0, 0)
		self.boardstyle[5][9] = (150, 0, 0)
		self.boardstyle[5][8] = (150, 0, 0)
		self.boardstyle[5][7] = (150, 0, 0)
		self.boardstyle[5][6] = (150, 0, 0)

		self.boardstyle[4][1] = (110, 110, 110)
		self.boardstyle[4][2] = (110, 110, 110)
		self.boardstyle[4][3] = (110, 110, 110)
		self.boardstyle[4][7] = (110, 110, 110)
		self.boardstyle[4][8] = (110, 110, 110)
		self.boardstyle[4][9] = (110, 110, 110)
		self.boardstyle[4][10] = (110, 110, 110)

		self.boardstyle[6][0] = (110, 110, 110)
		self.boardstyle[6][1] = (110, 110, 110)
		self.boardstyle[6][2] = (110, 110, 110)
		self.boardstyle[6][3] = (110, 110, 110)
		self.boardstyle[6][7] = (110, 110, 110)
		self.boardstyle[6][8] = (110, 110, 110)
		self.boardstyle[6][9] = (110, 110, 110)

		self.boardstyle[0][4] = (110, 110, 110)
		self.boardstyle[1][4] = (110, 110, 110)
		self.boardstyle[2][4] = (110, 110, 110)
		self.boardstyle[3][4] = (110, 110, 110)
		self.boardstyle[4][4] = (110, 110, 110)
		self.boardstyle[6][4] = (110, 110, 110)
		self.boardstyle[7][4] = (110, 110, 110)
		self.boardstyle[8][4] = (110, 110, 110)
		self.boardstyle[9][4] = (110, 110, 110)

		self.boardstyle[1][6] = (110, 110, 110)
		self.boardstyle[2][6] = (110, 110, 110)
		self.boardstyle[3][6] = (110, 110, 110)
		self.boardstyle[4][6] = (110, 110, 110)
		self.boardstyle[6][6] = (110, 110, 110)
		self.boardstyle[7][6] = (110, 110, 110)
		self.boardstyle[8][6] = (110, 110, 110)
		self.boardstyle[9][6] = (110, 110, 110)
		self.boardstyle[10][6] = (110, 110, 110)

		self.boardstyle[0][5] = (110, 110, 110)
		self.boardstyle[10][5] = (110, 110, 110)
		self.boardstyle[5][0] = (110, 110, 110)
		self.boardstyle[5][10] = (110, 110, 110)

		self.isover = 0
		self.lastmove = ""

		self.datapath = "./"

		self.dice = tuple(array(2))
		self.lastdice = None

	def init(self, path):
		self.datapath = path

	def name(self):
		return unicode("M-Ä-D-N", "utf-8")

	def figure(self, piece):
		(gfx, color) = piece
		if color == "b":
			colorstr = "blue"
		elif color == "r":
			colorstr = "red"
		elif color == "g":
			colorstr = "green"
		elif color == "y":
			colorstr = "yellow"
		else:
			colorstr = "black"
		piecestr = "poeppel"
		return self.datapath + "madn/" + piecestr + "-" + colorstr + ".svg"

	def validatemove(self, fromcolor, frompos, topos):
		return 0

	# returns 'None' if not found or illegal given
	# returns 'topos' if given and legal, or calculated 'topos' if given as None
	def find_and_validate(self, frompos, topos, steps):
		#if topos:
		#	(x, y) = topos
		(oldx, oldy) = frompos
		print "validate moves from", frompos, "to", topos, "counting", steps
		(cx, cy) = (oldx, oldy)
		performedsteps = 0
		transitions = (
			((0, 4), (3, 4), (1, 0), None),
			((6, 4), (9, 4), (1, 0), None),
			((10, 4), None, (0, 1), None),

			((1, 6), (4, 6), (-1, 0), None),
			((7, 6), (10, 6), (-1, 0), None),
			((0, 6), None, (0, -1), None),

			((4, 1), (4, 4), (0, -1), None),
			((4, 7), (4, 10), (0, -1), None),
			((4, 0), None, (1, 0), None),

			((6, 0), (6, 3), (0, 1), None),
			((6, 6), (6, 9), (0, 1), None),
			((6, 10), None, (-1, 0), None),

			((0, 5), (3, 5), (1, 0), "y"),
			((10, 5), (9, 5), (-1, 0), "r"),
			((5, 0), (5, 3), (0, 1), "b"),
			((5, 10), (5, 9), (0, -1), "g"),

			((0, 5), None, (0, -1), "brg"),
			((10, 5), None, (0, 1), "bgy"),
			((5, 0), None, (1, 0), "rgy"),
			((5, 10), None, (-1, 0), "bry")
		)
		for step in range(steps):
			transited = 0
			for trans in transitions:
				if not transited:
					(first, last, rel, desc) = trans
					if not last:
						last = first
					(fx, fy) = first
					(lx, ly) = last
					(rx, ry) = rel
					if (cx >= fx and cx <= lx) or (cx >= lx and cx <= fx):
						if (cy >= fy and cy <= ly) or (cy >= ly and cy <= fy):
							valid = 1
							if desc:
								if color not in desc:
									valid = 0
							if valid:
								cx += rx
								cy += ry
								transited = 1
								performedsteps += 1
								print "- transit to", (cx, cy)
		print "arrived at", (cx, cy), "should be", topos
		if not topos:
			if performedsteps != steps:
				print "steps", steps, "performed", performedsteps
				return None
		return (cx, cy)

	def trymove(self, frompos, topos):
		print "tryomove -- ", frompos, topos, "using last dice", self.lastdice
		(x, y) = topos
		(oldx, oldy) = frompos
		if not self.board[oldy][oldx]:
			return 0
		if frompos == topos:
			self.lastdice = None
			return 0
		(gfx, color) = self.board[oldy][oldx]
		
		if self.lastdice == 1:
			if frompos == (1, 1) or frompos == (0, 1) or frompos == (1, 0) or frompos == (0, 0):
				if topos == (0, 4):
					#self.domove(frompos, topos)
					return 1

		(cx, cy) = self.find_and_validate(frompos, topos, self.lastdice)

		if cx == x and cy == y:
			print "goooooooal!"
			return 1

#			if cy == 4:
#				if cx >= 0 and cx < 4:
#					cx += 1
#				elif cx == 4:
#					cy -= 1
#				elif cx >= 6 and cx < 10:
#					cx += 1
#				elif cx == 10:
#					cy += 1
#			elif cy == 6:
#				if cx <= 10 and cx > 6:
#					cx -= 1
#				elif cx == 6:
#					cy += 1
#				elif cx <= 4 and cx > 0:
#					cx -= 1
#				elif cx == 0:
#					cy -= 1
		#if oldy == 4 and y == 4:
		#	if oldx >= 0 and x <= 4 and x > oldx:
		#		return 1
		#	if oldx >= 6 and x <= 10 and x > oldx:
		#		return 1
		self.lastdice = None
		return 0

	def aimove(self):
		self.lastdice = None
		self.rolldice()
		return (0, None, None)

	def domove(self, frompos, topos):
		(oldx, oldy) = frompos
		(x, y) = topos
		print "** DOMOVE", frompos, topos
		print self.board[oldy][oldx]
		self.board[y][x] = self.board[oldy][oldx]
		self.board[oldy][oldx] = None

	def over(self):
		return self.isover

	def toggleplayer(self):
		pass

	def rolldice(self):
		if self.lastdice is not None:
			return 0
		print "### ROLL DICE ###"
		#for dice in self.dice:
		#	if dice == 0:
		#		dice = random.randint(0, 6)
		self.dice = []

		for trial in range(3):
			dice = random.randint(1, 6)
			#self.dice = tuple(array(dice))
			self.dice.append(dice)

			usable = 0
			for j in range(self.height):
				for i in range(self.width):
					field = self.board[j][i]
					if field:
						(gfx, color) = field
						if color == "y":
							print "CHECK move ability", i, j
							ret = self.find_and_validate((i, j), None, dice)
							print " =>", ret
							if ret is not None:
								usable = 1
			if usable:
				self.lastdice = dice
				return 1
			if dice == 1:
				self.lastdice = dice
				return 1
			#if dice != 6:
			#	return 0

		return 0

ggzboardgame = Game()
