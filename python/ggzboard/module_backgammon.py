# GGZBoard Backgammon: Backgammon game module for the GGZBoard container
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 13
		self.height = 11
		self.swaptiles = 0
		self.autoscaletiles = 1
		self.setonly = 0
		self.intersections = 0
		self.noemptytiles = 1
		self.players = 2
		self.turnplayer = 0

		self.boardbackground = "backgammon/bg.svg"

		self.boardhints = None

		self.boardstyle = (None)
		self.boardstyle = resize(self.boardstyle, (self.height, self.width))
		# FIXME: switch width/height in all others

		for i in range(self.width):
			for j in range(self.height):
				color = None
				if i == 6 or j == 5:
					continue
				if i % 2 == (((i < 6) and (j > 5)) or ((i > 6) and (j < 5))):
					color = (230, 230, 230)
				else:
					color = (0, 0, 0)
				self.boardstyle[j][i] = color

		self.board = (None)
		self.board = resize(self.board, (self.height, self.width))

		self.board[0][12] = ("piece", "b")
		self.board[1][12] = ("piece", "b")
		self.board[9][12] = ("piece", "w")
		self.board[10][12] = ("piece", "w")

		self.board[0][7] = ("piece", "w")
		self.board[1][7] = ("piece", "w")
		self.board[2][7] = ("piece", "w")
		self.board[3][7] = ("piece", "w")
		self.board[4][7] = ("piece", "w")
		self.board[6][7] = ("piece", "b")
		self.board[7][7] = ("piece", "b")
		self.board[8][7] = ("piece", "b")
		self.board[9][7] = ("piece", "b")
		self.board[10][7] = ("piece", "b")

		self.board[0][4] = ("piece", "w")
		self.board[1][4] = ("piece", "w")
		self.board[2][4] = ("piece", "w")
		self.board[8][4] = ("piece", "b")
		self.board[9][4] = ("piece", "b")
		self.board[10][4] = ("piece", "b")

		self.board[0][0] = ("piece", "b")
		self.board[1][0] = ("piece", "b")
		self.board[2][0] = ("piece", "b")
		self.board[3][0] = ("piece", "b")
		self.board[4][0] = ("piece", "b")
		self.board[6][0] = ("piece", "w")
		self.board[7][0] = ("piece", "w")
		self.board[8][0] = ("piece", "w")
		self.board[9][0] = ("piece", "w")
		self.board[10][0] = ("piece", "w")

		self.isover = 0
		self.lastmove = ""

		self.datapath = "./"

		self.dice = tuple(array(2))
		self.lastdice = None

		self.playercolours = None
		self.winner = None
		self.help = None

		self.selection = None

		self.boardwidth = 50 # !!!
		self.boardheight = 50 # !!!

	# !!!
	def selectpiece(self, placepos):
		return

	def init(self, path):
		self.datapath = path

	def name(self):
		return _("Backgammon")

	def figure(self, piece):
		(gfx, color) = piece
		if color == "w":
			colorstr = "white"
		else:
			colorstr = "black"
		piecestr = "piece"
		return self.datapath + "reversi/" + piecestr + "-" + colorstr + ".svg"

	def validatemove(self, fromcolor, frompos, topos):
		(oldx, oldy) = frompos
		(x, y) = topos

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

		if valid == 0:
			print reason
		return valid

	def trymove(self, frompos, topos):
		ret = 0

		(x, y) = topos
		(oldx, oldy) = frompos
		valid = self.validatemove("w", frompos, topos)

		if valid:
			print "TRYMOVE", frompos, topos
			ret = 1
			self.lastmove = "w"
		return ret

	def aimove(self):
		return (0, None, None)

		ret = 0
		topos = None
		frompos = None
		#value = 0
		for j in range(self.height):
			for i in range(self.width):
				pass
				#f = self.board[j][i]
				#if not f:
				#	if value < 1:
				#		topos = (i, j)
				#		if self.validatemove("b", frompos, topos):
				#			print "AIMOVE(1)", frompos, topos
				#			ret = 1
				#			value = 1
		if ret == 0:
			self.isover = 1
		return (ret, frompos, topos)

	def domove(self, frompos, topos):
		if self.validatemove(self.lastmove, frompos, topos):
			(oldx, oldy) = frompos
			(x, y) = topos
			tmp = self.board[oldy][oldx]
			self.board[y][x] = tmp
			self.board[oldy][oldx] = None

	def over(self):
		return self.isover

	def toggleplayer(self):
		if self.lastmove == "w":
			self.lastmove = "b"
		else:
			self.lastmove = "w"
		self.turnplayer = (self.turnplayer + 1) % self.players

	def rolldice(self):
		self.dice = []
		self.lastdice = None

		for trial in range(2):
			dice = random.randint(1, 6)
			self.dice.append(dice)

		return 1

ggzboardgame = Game()

