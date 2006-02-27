#!/usr/bin/env python
# GGZBoard Arimaa: Arimaa game module for the GGZBoard container
# Copyright (C) 2005-2006 Josef Spillner <josef@ggzgamingzone.org>
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

		self.lastposition = None
		self.allowopponentmove = None
		self.pendingpush = None
		self.pendingpushorigin = None

		self.micromoves = 0

		print "INIT!"
		self.autoinit()

	def autoinit(self):
		i = 0
		for piece in self.piecelist["gold"]:
			self.selected = piece
			self.domove(None, (i % 8, i / 8 + 6))
			i += 1
		i = 0
		for piece in self.piecelist["silver"]:
			self.selected = piece
			self.domove(None, (i % 8, 1 - i / 8))
			i += 1
		###
		self.domove((0, 1), (0, 2))
		self.domove((0, 0), (0, 3))
		self.domove((1, 0), (2, 3))
		self.domove((0, 7), (4, 3))

	def selectpiece(self, placepos):
		if not self.placements:
			return
		print "player piece select!"
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

		valid = 1
		reason = ""

		f = self.board[oldy][oldx]
		if not f:
			return 0
		(gfx, color) = f

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
			(piece2, color2) = self.board[y][x]
			if color == color2:
				valid = 0
				reason = "cannot jump on another piece of same colour"
				# FIXME: yes one can, but only for pushing them
			elif color != fromcolor:
				valid = 0
				reason = "not your piece for pushing"
			elif self.weight(piece2) < self.weight(gfx):
				self.boardhints = None
				self.boardhints = resize(self.boardstyle, (self.width, self.height))
				self.boardhints[y][x] = 1
				self.allowopponentmove = (x, y)
				valid = 0 # FIXME: better semantics ("delayed move")
				reason = "delayed-due-to-push"
				self.pendingpushorigin = (oldx, oldy)
				self.pendingpush = (x, y)
			else:
				valid = 0
				reason = "pushing stronger pieces is not allowed"
		else:
			if fromcolor == "":
				fromcolor = color
			if color != fromcolor:
				if (oldx, oldy) == self.allowopponentmove:
					print "yup, temporarily allowed"
				else:
					valid = 0
					# FIXME: this happens when pulling sometimes?
					reason = "not your piece"

		if valid == 1:
			print "try this MOVE", (oldx, oldy), (x, y)
			if abs(oldx - x) > 1 or abs(oldy - y) > 1 or abs(abs(oldx - x) - abs(oldy - y)) != 1:
				valid = 0
				reason = "move outside of bounds"
			if gfx == "rabbit":
				backwards = 0
				if color == "gold" and y > oldy:
					backwards = 1
				if color == "silver" and y < oldy:
					backwards = 1
				if backwards == 1:
					valid = 0
					reason = "rabbits must not move backwards"

		if valid == 1:
			bcolor = "gold"
			if fromcolor == "gold":
				bcolor = "silver"

			if self.borders(oldx - 1, oldy, bcolor, gfx):
				valid = 0
			if self.borders(oldx + 1, oldy, bcolor, gfx):
				valid = 0
			if self.borders(oldx, oldy - 1, bcolor, gfx):
				valid = 0
			if self.borders(oldx, oldy + 1, bcolor, gfx):
				valid = 0
			if self.borders(oldx - 1, oldy, fromcolor, None):
				valid = 1
			if self.borders(oldx + 1, oldy, fromcolor, None):
				valid = 1
			if self.borders(oldx, oldy - 1, fromcolor, None):
				valid = 1
			if self.borders(oldx, oldy + 1, fromcolor, None):
				valid = 1
			if valid == 0:
				reason = "piece is stuck due to bordering piece of other colour"

		if valid == 0:
			print reason
		return valid

	def borders(self, x, y, c, gfx):
		if x < 0 or y < 0 or x >= self.width or y >= self.height:
			return
		f = self.board[y][x]
		if f:
			(piece, color) = f
			if color == c:
				if gfx:
					if self.weight(gfx) < self.weight(piece):
						return 1
				else:
					return 1
		return 0

	def trymove(self, frompos, topos):
		ret = 0
		valid = 0

		if not self.placements:
			(oldx, oldy) = frompos
			(x, y) = topos
			valid = self.validatemove("gold", frompos, topos)
		else:
			print "XXX SELECTED", self.selected
			if self.selected:
				if self.selected in self.piecelist["gold"]:
					(x, y) = topos
					if self.board[y][x] is None:
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

	# FIXME: reset allowopponentmove somewhen?
	def initmove(self, x, y):
		if self.boardhints:
			if self.boardhints[y][x] == 1:
				self.boardhints = None
				self.boardhints = resize(self.boardstyle, (self.width, self.height))
				if self.allowopponentmove:
					self.checkpush(x, y, x + 1, y)
					self.checkpush(x, y, x - 1, y)
					self.checkpush(x, y, x, y - 1)
					self.checkpush(x, y, x, y + 1)
				else:
					(oldx, oldy) = self.lastposition
					self.boardhints[oldy][oldx] = 1
					self.allowopponentmove = (x, y)

	def checkpush(self, x, y, px, py):
		if px < 0 or py < 0 or px >= self.width or py >= self.height:
			return
		(piece, color) = self.board[y][x]
		if self.board[py][px] == None:
			# FIXME: unify rabbit checking code?
			backwards = 0
			if piece == "rabbit":
				if color == "gold" and py > y:
					backwards = 1
				if color == "silver" and py < y:
					backwards = 1
			if backwards == 0:
				self.boardhints[py][px] = 1

	def weight(self, piece):
		if piece == "elephant":
			return 10
		if piece == "camel":
			return 8
		if piece == "horse":
			return 6
		if piece == "dog":
			return 4
		if piece == "cat":
			return 2
		if piece == "rabbit":
			return 0

	def checkpull(self, x, y, px, py):
		if x < 0 or y < 0 or x >= self.width or y >= self.height:
			return
		(piece, color) = self.board[py][px]
		print "checkpull", x, y, "by", (piece, color)
		if self.board[y][x] != None:
			(piece2, color2) = self.board[y][x]
			print "found victim", (piece2, color2)
			if color != color2:
				if self.weight(piece2) < self.weight(piece):
					self.boardhints[y][x] = 1
					print "strike!"

	def checktraps(self):
		for pos in ((2, 2), (2, 5), (5, 2), (5, 5)):
			(x, y) = pos
			f = self.board[y][x]
			if f:
				print "trap!", pos
				trapped = 1
				(piece, color) = f
				if self.borders(x - 1, y, color, None):
					trapped = 0
				if self.borders(x + 1, y, color, None):
					trapped = 0
				if self.borders(x, y - 1, color, None):
					trapped = 0
				if self.borders(x, y + 1, color, None):
					trapped = 0
				if trapped:
					self.board[y][x] = None

	def domove(self, frompos, topos):
		if not self.placements:
			(oldx, oldy) = frompos
			(x, y) = topos
			self.board[y][x] = self.board[oldy][oldx]
			self.board[oldy][oldx] = None

			self.boardhints = None
			self.boardhints = resize(self.boardstyle, (self.width, self.height))
			# FIXME: check for pull only if no push is finished?
			self.checkpull(oldx - 1, oldy, x, y)
			self.checkpull(oldx + 1, oldy, x, y)
			self.checkpull(oldx, oldy - 1, x, y)
			self.checkpull(oldx, oldy + 1, x, y)

			self.lastposition = (oldx, oldy)

			if self.pendingpush == frompos:
				print "finish-push"
				#self.domove(...)
				(ox, oy) = self.pendingpushorigin
				self.pendingpush = None
				self.board[oldy][oldx] = self.board[oy][ox]
				self.board[oy][ox] = None

			self.checktraps()
		else:
			if self.selected:
				(piece, color) = self.selected
				(x, y) = topos
				if (y != 6 and y != 7) and color == "gold":
					return
				if (y != 0 and y != 1) and color == "silver":
					return
				for i in range(len(self.piecelist[color])):
					if self.piecelist[color][i] == self.selected:
						self.piecelist[color][i] = None
						break
				self.board[y][x] = self.selected
				self.selected = None

				print "XXX", (piece, color)
				switchstate = 0
				if len([x for x in self.piecelist["gold"] if x]):
					switchstate += 1
				if len([x for x in self.piecelist["silver"] if x]):
					switchstate += 1
				if switchstate == 0:
					print "switch state: game is in 2nd phase (piece moves) now"
					self.placements = 0
					self.setonly = 0

	def over(self):
		return self.isover

	def toggleplayer(self):
		self.turnplayer = (self.turnplayer + 1) % self.players
		# FIXME: micro-moves don't seem to be possible with current ggzboard model
		# FIXME: arimaa needs 1-4 micro-moves per turn (steps)
		# FIXME: pushing/pulling a piece takes 2 micro-moves
		#self.micromoves += 1
		#if self.micromoves == 4:
		#	self.micromoves = 0
		#	self.turnplayer = (self.turnplayer + 1) % self.players

ggzboardgame = Game()
