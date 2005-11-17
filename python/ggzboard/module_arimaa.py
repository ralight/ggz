#!/usr/bin/env python
# GGZBoard Arimaa: Arimaa game module for the GGZBoard container
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 8
		self.height = 8
		self.swaptiles = 0
		self.autoscaletiles = 0 # due to images
		self.setonly = 0
		self.intersections = 0
		self.noemptytiles = 0
		self.players = 2
		self.turnplayer = 0

		#self.boardstyle = None
		self.boardhints = None

		self.boardstyle = (None)
		self.boardstyle = resize(self.boardstyle, (self.width, self.height))

		for j in range(self.height):
			for i in range(self.width):
				self.boardstyle[i][j] = "arimaa/cell.png"
		self.boardstyle[2][2] = "arimaa/celltrap.png"
		self.boardstyle[2][5] = "arimaa/celltrap.png"
		self.boardstyle[5][2] = "arimaa/celltrap.png"
		self.boardstyle[5][5] = "arimaa/celltrap.png"

		self.boardwidth = 58
		self.boardheight = 58

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.isover = 0

		self.datapath = "./"

		self.dice = None
		self.playercolours = None
		self.winner = None
		self.help = None

		# ...
		self.placements = 1
		self.selection = None
		self.setonly = 1
		self.selected = None

		piecelist = []
		for i in range(8):
			piecelist.append("rabbit")
		for i in range(2):
			piecelist.append("cat")
		for i in range(2):
			piecelist.append("dog")
		for i in range(2):
			piecelist.append("horse")
		piecelist.append("camel")
		piecelist.append("elephant")

		pl1 = piecelist[:]
		pl2 = piecelist[:]
		for i in range(len(piecelist)):
			pl1[i] = (pl1[i], "silver")
			pl2[i] = (pl2[i], "gold")

		self.piecelist = {}
		self.piecelist["silver"] = pl1
		self.piecelist["gold"] = pl2

	def selectpiece(self, placepos):
		if not self.placements:
			return
		print "..."
		self.selection = []
		for piece in self.piecelist["gold"]:
			if piece:
				self.selection.append(piece)

	def init(self, path):
		self.datapath = path

	def name(self):
		return _("Arimaa")

	def figure(self, piece):
		(gfx, color) = piece
		colorstr = color
		piecestr = gfx
		return self.datapath + "arimaa/" + piecestr + "_" + colorstr + ".png"

	def validatemove(self, fromcolor, frompos, topos):
		if frompos:
			(oldx, oldy) = frompos
		(x, y) = topos

		# XXX
		return 1

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
				pass
				# ???

		if valid == 0:
			print reason
		return valid

	def trymove(self, frompos, topos):
		ret = 0
		valid = 0

		if not self.placements:
			(oldx, oldy) = frompos
			(x, y) = topos
			valid = self.validatemove("gold", frompos, topos)
		else:
			if self.selected:
				if self.selected in self.piecelist["gold"]:
					valid = 1

		if valid:
			#(figure, color) = self.board[oldy][oldx]
			print "TRYMOVE", frompos, topos
			ret = 1

		return ret

	def aimove(self):
		# FIXME: find???
		#self.isover = 1
		return (0, 0, 0)

		frompos = (fromval % 8, fromval / 8)
		topos = (toval % 8, toval / 8)
		if ret == 0:
			self.isover = 1
		else:
			pass
			# FIXME: move
		return (ret, frompos, topos) 

	def domove(self, frompos, topos):
		if not self.placements:
			(oldx, oldy) = frompos
			(x, y) = topos
			self.board[y][x] = self.board[oldy][oldx]
			self.board[oldy][oldx] = None
		else:
			if self.selected:
				for i in range(len(self.piecelist["gold"])):
					if self.piecelist["gold"][i] == self.selected:
						self.piecelist["gold"][i] = None
						break
				(x, y) = topos
				self.board[y][x] = self.selected
				self.selected = None

				switchstate = 1
				for piece in self.piecelist["gold"]:
					if piece:
						switchstate = 0
				#for piece in self.piecelist["silver"]:
				#	if piece:
				#		switchstate = 0
				if switchstate:
					self.placements = 0
					self.setonly = 0

	def over(self):
		return self.isover

	def toggleplayer(self):
		#pass
		self.turnplayer = (self.turnplayer + 1) % self.players

ggzboardgame = Game()
