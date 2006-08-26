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
		self.players = 4
		self.turnplayer = 0

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

		self.boardhints = (None)
		self.boardhints = resize(self.boardstyle, (self.width, self.height))

		self.isover = 0
		self.lastmove = ""

		self.datapath = "./"

		self.dice = tuple(array(3))
		self.lastdice = None

		self.depots = {}
		self.depots["y"] = ((1, 1), (0, 1), (1, 0), (0, 0))
		self.depots["b"] = ((9, 0), (10, 0), (9, 1), (10, 1))
		self.depots["r"] = ((9, 9), (9, 10), (10, 9), (10, 10))
		self.depots["g"] = ((0, 9), (1, 9), (0, 10), (1, 10))

		self.starts = {}
		self.starts["y"] = (0, 4)
		self.starts["b"] = (6, 0)
		self.starts["r"] = (10, 6)
		self.starts["g"] = (4, 10)

		self.finishes = {}
		self.finishes["y"] = ((1, 5), (2, 5), (3, 5), (4, 5))
		self.finishes["b"] = ((5, 1), (5, 2), (5, 3), (5, 4))
		self.finishes["r"] = ((6, 5), (7, 5), (8, 5), (9, 5))
		self.finishes["g"] = ((5, 6), (5, 7), (5, 8), (5, 9))

		self.playercolours = {}
		self.playercolours[0] = (150, 150, 0)
		self.playercolours[1] = (0, 0, 200)
		self.playercolours[2] = (150, 0, 0)
		self.playercolours[3] = (0, 150, 0)

		self.winner = None

		self.help = []
		self.help.append(_("Ludo is a game for up to four players."))
		self.help.append(_("After rolling dice, each player can either move a piece"))
		self.help.append(_("out of the depot, onto the start field, or move a piece"))
		self.help.append(_("already outside clock-wise until it reaches the end field."))
		self.help.append(_("Opponent pieces can be beaten and are put back into the depot."))
		self.help.append(_("To come out of a depot, a die must show a 1."))

		self.selection = None
		self.boardwidth = 50 # !!!
		self.boardheight = 50 # !!!

	def init(self, path):
		self.datapath = path

	def name(self):
		return _("Ludo")

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
		(gfx, color) = self.board[oldy][oldx]
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
								piece = self.board[cy][cx]
								if piece:
									(gfx2, color2) = piece
									if color == color2:
										if steps == performedsteps:
											print "- crash into own piece!"
											performedsteps -= 1
		print "arrived at", (cx, cy), "should be", topos
		if not topos:
			if performedsteps != steps:
				print "steps", steps, "performed", performedsteps
				return None
		return (cx, cy)

	def trymove(self, frompos, topos):
		self.boardhints = (None)
		self.boardhints = resize(self.boardstyle, (self.width, self.height))

		print "trymove --", frompos, topos, "using last dice", self.lastdice
		turncolours = ("y", "b", "r", "g")
		turncolour = turncolours[self.turnplayer]
		print "trymove -- player turn", self.turnplayer, "player colour", turncolour
		if not self.lastdice:
			return 0
		(x, y) = topos
		(oldx, oldy) = frompos
		if not self.board[oldy][oldx]:
			print "no figure at that position!"
			return 0
		if frompos == topos:
			#self.lastdice = None
			print "no real move!"
			return 0
		(gfx, color) = self.board[oldy][oldx]
		if color != turncolour:
			print "wrong colour!"
			return 0
		if self.board[y][x]:
			(gfx2, color2) = self.board[y][x]
			if color2 == color:
				print "suicide!"
				return 0
		
		if self.lastdice == 1:
			if frompos in self.depots[turncolour]:
				if topos == self.starts[turncolour]:
					print "come out!"
					return 1

		(cx, cy) = self.find_and_validate(frompos, topos, self.lastdice)

		if cx == x and cy == y:
			print "goooooooal!"
			return 1

		#self.lastdice = None
		return 0

	def aimove(self):
		print "####################################### AI MOVE dice=", self.lastdice
		turncolours = ("y", "b", "r", "g")
		turncolour = turncolours[self.turnplayer]

		if not self.lastdice:
			print "ERROR - no dice has been rolled, using value 1"
			self.lastdice = 1
			# FIXME: receive from network, and disallow otherwise

		if self.lastdice == 1:
			for depot in self.depots[turncolour]:
				ret = self.trymove(depot, self.starts[turncolour])
				if ret == 1:
					return (1, depot, self.starts[turncolour])

		for j in range(self.height):
			for i in range(self.width):
				field = self.board[j][i]
				if field:
					(gfx, color) = field
					if color == turncolour:
						print "AI CHECK move ability", i, j
						ret = self.find_and_validate((i, j), None, self.lastdice)
						print " =>", ret
						if ret is not None:
							(x, y) = ret
							
							if self.board[y][x]:
								(gfx2, color2) = self.board[y][x]
								if color2 == color:
									print "suicide!"
									ret = 0

							if ret:
								print "AI move to", x, y
								return (1, (i, j), (x, y))

		self.lastdice = None
		#self.rolldice()
		return (0, None, None)

	def domove(self, frompos, topos):
		(oldx, oldy) = frompos
		(x, y) = topos
		print "** DOMOVE", frompos, topos
		print self.board[oldy][oldx]
		(gfx, color) = self.board[oldy][oldx]
		if self.board[y][x]:
			(gfx2, color2) = self.board[y][x]
			print "** KILL", self.board[y][x]
			for depot in self.depots[color2]:
				(i, j) = depot
				if self.board[j][i] == None:
					self.board[j][i] = (gfx2, color2)
					break
		self.board[y][x] = self.board[oldy][oldx]
		self.board[oldy][oldx] = None

		won = 1
		for finish in self.finishes[color]:
			(i, j) = finish
			if self.board[j][i] == None:
				won = 0
		if won:
			self.isover = 1
			self.winner = self.turnplayer

	def over(self):
		return self.isover

	def toggleplayer(self):
		self.turnplayer = (self.turnplayer + 1) % self.players

	def rolldice(self):
#		if self.lastdice is not None:
#			return 0
		turncolours = ("y", "b", "r", "g")
		turncolour = turncolours[self.turnplayer]
		print "### ROLL DICE ###"
		self.dice = []
		self.lastdice = None

		for trial in range(3):
			dice = random.randint(1, 6)
			self.dice.append(dice)

			usable = 0
			for j in range(self.height):
				for i in range(self.width):
					field = self.board[j][i]
					if field:
						(gfx, color) = field
						if color == turncolour:
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

	def initmove(self, x, y):
		turncolours = ("y", "b", "r", "g")
		turncolour = turncolours[self.turnplayer]

		self.boardhints = (None)
		self.boardhints = resize(self.boardstyle, (self.width, self.height))

		if not self.lastdice:
			print "ERROR - no dice has been rolled, using value 1"
			self.lastdice = 1
			# FIXME: receive from network, and disallow otherwise

		field = self.board[y][x]
		if field:
			(gfx, color) = field
			if color == turncolour:
				print "FIND target", x, y
				ret = self.find_and_validate((x, y), None, self.lastdice)
				print " =>", ret
				if ret is not None:
					print "TARGET", ret
					(x2, y2) = ret
					self.boardhints[y2][x2] = 1

			if self.lastdice == 1:
				if (x, y) in self.depots[turncolour]:
					(x2, y2) = self.starts[turncolour]
					self.boardhints[y2][x2] = 1

ggzboardgame = Game()
