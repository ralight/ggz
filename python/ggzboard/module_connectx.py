# GGZBoard ConnectX: ConnectX game module for the GGZBoard container
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 4
		self.height = 4
		self.swaptiles = 0
		self.autoscaletiles = 1
		self.setonly = 1
		self.intersections = 0
		self.noemptytiles = 0
		self.players = 2
		self.turnplayer = 0

		self.boardstyle = None
		self.boardhints = None

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.isover = 0

		self.datapath = "./"

		self.dice = None
		self.playercolours = None
		self.winner = None
		self.help = None

		self.selection = None

	# !!!
	def selectpiece(self, placepos):
		return

	def init(self, path):
		self.datapath = path

	def name(self):
		return _("ConnectX")

	def figure(self, piece):
		(gfx, color) = piece
		if color == "w":
			piecestr = "player1"
		else:
			piecestr = "player2"
		return self.datapath + "connectx/" + piecestr + ".svg"

	def validatemove(self, topos):
		(x, y) = topos

		valid = 1
		reason = ""

		if x < 0 or x >= self.width or y < 0 or y >= self.height:
			valid = 0
			reason = "out of bounds"
		elif self.board[0][x] is not None:
			valid = 0
			reason = "cannot move to already full column"
		else:
			valid = 1

		if valid == 0:
			print reason
		return valid

	def trymove(self, frompos, topos):
		return self.validatemove(topos)

	def aimove(self):
		topos = None

		if self.over():
			return (0, None, None)

		while not topos:
			i = random.randint(0, self.width - 1)
			if self.board[0][i] == None:
				topos = (i, 0)

		return (1, None, topos)

	def domove(self, frompos, topos):
		if self.validatemove(topos):
			(x, y) = topos
			for j in range(y, self.height):
				if self.board[j][x] == None:
					y = j
				else:
					break

			lastmove = "b"
			if self.turnplayer == 1:
				lastmove = "w"
			self.board[y][x] = ("piece", lastmove)

			self.over()

	def over(self):
		if self.isover:
			return self.isover

		self.isover = 1
		for i in range(self.width):
			if self.board[0][i] == None:
				self.isover = 0

		for j in range(self.height):
			first = self.board[j][0]
			if not first:
				continue
			for i in range(self.width):
				if self.board[j][i] != first:
					first = None
			if first:
				self.winner = self.turnplayer
				self.isover = 1
				print "4 in a row!", self.winner

		for i in range(self.width):
			first = self.board[0][i]
			if not first:
				continue
			for j in range(self.height):
				if self.board[j][i] != first:
					first = None
			if first:
				self.winner = self.turnplayer
				self.isover = 1
				print "4 in a column!", self.winner

		return self.isover

	def toggleplayer(self):
		self.turnplayer = (self.turnplayer + 1) % self.players

ggzboardgame = Game()

