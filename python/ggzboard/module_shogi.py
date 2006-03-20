#!/usr/bin/env python
# GGZBoard Shogi: Shogi game module for the GGZBoard container
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random
import os
import re

class Game:
	def __init__(self):
		self.width = 9
		self.height = 9
		self.swaptiles = 0
		self.autoscaletiles = 1
		self.setonly = 0
		self.intersections = 0
		self.noemptytiles = 0
		self.players = 2
		self.turnplayer = 0

		self.boardstyle = None

		self.boardhints = (None)
		self.boardhints = resize(self.boardstyle, (self.width, self.height))

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.board[0][0] = ("lance", "w")
		self.board[0][1] = ("knight", "w")
		self.board[0][2] = ("silver-general", "w")
		self.board[0][3] = ("gold-general", "w")
		self.board[0][4] = ("king", "w")
		self.board[0][5] = ("gold-general", "w")
		self.board[0][6] = ("silver-general", "w")
		self.board[0][7] = ("knight", "w")
		self.board[0][8] = ("lance", "w")

		self.board[1][1] = ("bishop", "w")
		self.board[1][7] = ("rook", "w")

		for i in range(9):
			self.board[2][i] = ("pawn", "w")

		# mirror "w" to "b"
		for j in range(3):
			for i in range(9):
				if self.board[j][i]:
					(gfx, color) = self.board[j][i]
					self.board[8 - j][8 - i] = (gfx, "b")

		self.isover = 0
		self.lastmove = ""

		self.datapath = "./"

		self.dice = None
		self.playercolours = None
		self.winner = None
		self.help = None

		self.selection = None

		self.runningshogi = None
		self.pin = None
		self.pout = None

		self.generatedmove = None

	# !!!
	def selectpiece(self, placepos):
		return

	def init(self, path):
		self.datapath = path

		if not self.runningshogi:
			print "Launch local AI!"
			self.runningshogi = 1
			(self.pout, self.pin) = os.popen2("gnushogi")

			s = self.pin.readline()
			s = s.strip()
			print "shogi greeting >>>", s

			self.pout.write("white\n")
			self.pout.flush()

	def name(self):
		return _("Shogi")

	def figure(self, piece):
		(gfx, color) = piece
		if color == "w":
			piecestr = gfx
		else:
			piecestr = "b-" + gfx
		#return self.datapath + "reversi/" + piecestr + "-" + colorstr + ".svg"
		return self.datapath + "shogi/" + piecestr + ".svg"

	# white
	# 5c5d
	# 3. 5c5d 241150
	# 3. ... 7i7h 291210
	def validatemove(self, fromcolor, frompos, topos):
		if not frompos or not topos:
			print "(validatemove: null arguments)"
			return 0

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
			#valid = 0
			#reason = "cannot move to occupied square"
			print "yo, capture him :-)"

		if valid == 0:
			print reason
		return valid

	def trymove(self, frompos, topos):
		ret = 0

		self.boardhints = (None)
		self.boardhints = resize(self.boardstyle, (self.width, self.height))

		(oldx, oldy) = frompos
		(x, y) = topos
		valid = self.validatemove("w", frompos, topos)

		if valid:
			if self.runningshogi:
				print "<<< (write)"
				movestr = str(oldx + 1) + chr(oldy + 97) + str(x + 1) + chr(y + 97)
				self.pout.write(movestr + "\n")
				self.pout.flush()
				print "---"
				answer = None
				accepted = 0
				while not answer:
					s = self.pin.readline()
					s = s.strip()
					print ">>>", s
					if "Illegal" in s:
						answer = 1
						accepted = 0
					elif "mates" in s:
						answer = 1
						accepted = 0
					else:
						accepted = 1
						answer = 1
						# 1 line with AI move follows here
						# it is read in aimove()!
				print "done!"
				print "Move accepted?", accepted
				valid = accepted

		if valid:
			print "TRYMOVE", frompos, topos
			ret = 1
		return ret

	def aimove(self):
		ret = 1
		frompos = None
		topos = None

		s = self.pin.readline()
		s = s.strip()
		print ">>>", s

		r_ai = re.compile("(\d+)\. \.\.\. (\d\w\d\w) \d+")
		m_ai = r_ai.match(s)

		r_aidrop = re.compile("(\d+)\. \.\.\. (\S\*\d\w) \d+")
		m_aidrop = r_aidrop.match(s)

		# FIXME: is this piece promotion?
		r_aicapt = re.compile("(\d+)\. \.\.\. (\d\w\d\w)\+ \d+")
		m_aicapt = r_aicapt.match(s)

		if m_ai:
			gmove = m_ai.group(2)
			print "AI MOVE:", gmove
			oldx = int(gmove[0]) - 1
			oldy = ord(gmove[1]) - 97
			x = int(gmove[2]) - 1
			y = ord(gmove[3]) - 97
			frompos = (oldx, oldy)
			topos = (x, y)
		elif m_aidrop:
			gdrop = m_aidrop.group(2)
			print "AI DROP:", gdrop
			piece = gdrop[0]
			x = int(gdrop[2]) - 1
			y = ord(gdrop[3]) - 97
			# FIXME: this is a quick piece dropping hack
			if piece == "P":
				piecestr = "pawn"
			elif piece == "K":
				piecestr = "knight"
			elif piece == "L":
				piecestr = "lance"
			elif piece == "S":
				piecestr = "silver-general"
			elif piece == "G":
				piecestr = "gold-general"
			elif piece == "R":
				piecestr = "rook"
			elif piece == "B":
				piecestr = "bishop"
			else:
				print "drop not understood!"
				ret = 0
			if ret == 1:
				self.board[y][x] = (piecestr, "b")
		elif m_aicapt:
			gcapt = m_aicapt.group(2)
			print "AI CAPTURE:", gcapt
			oldx = int(gcapt[0]) - 1
			oldy = ord(gcapt[1]) - 97
			x = int(gcapt[2]) - 1
			y = ord(gcapt[3]) - 97
			frompos = (oldx, oldy)
			topos = (x, y)
		else:
			print "AI MOVE fubared!"
			ret = 0

		if ret == 0:
			self.isover = 1

		return (ret, frompos, topos)

	def domove(self, frompos, topos):
		if self.validatemove(self.lastmove, frompos, topos):
			(oldx, oldy) = frompos
			(x, y) = topos
			self.board[y][x] = self.board[oldy][oldx]
			self.board[oldy][oldx] = None

	def over(self):
		return self.isover

	def toggleplayer(self):
		if self.lastmove == "w":
			self.lastmove = "b"
		else:
			self.lastmove = "w"
		self.turnplayer = (self.turnplayer + 1) % self.players

	def initmove(self, x, y):
		self.boardhints = (None)
		self.boardhints = resize(self.boardstyle, (self.width, self.height))

		field = self.board[y][x]
		if field:
			(gfx, color) = field
			if color == "w":
				print "FIND targets for", gfx, "at", (x, y)
				if gfx == "king":
					for j in range(-1, 2):
						for i in range(-1, 2):
							if x + i < 0 or x + i >= self.width:
								continue
							if y + j < 0 or y + j >= self.height:
								continue
							field = self.board[y + j][x + i]
							if not field:
								self.boardhints[y + j][x + i] = 1
							else:
								(gfx, color) = field
								if color == "b":
									self.boardhints[y + j][x + i] = 1
					# FIXME: no overflows, and opponent pieces (done?)

ggzboardgame = Game()

