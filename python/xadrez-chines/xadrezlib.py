#!/usr/bin/env python
# Backend library for the Chinese Chess game
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Proto:
	def __init__(self):
		self.move_invalid = 101
		self.move_ok = 102

class Xadrez:
	def __init__(self):
		self.proto = Proto()
		self.field = {}
		self.setup()

	def ai(self, x):
		pass

	def validate(self, oldx, oldy, x, y):
		proto = self.proto
		field = self.field

		ret = proto.move_invalid

		oldfigure = ""
		oldcolor = ""
		if field[oldx][oldy] is not "":
			if field[oldx][oldy].count(":") == 2:
				(oldfigure, oldcolor, oldoptions) = field[oldx][oldy].split(":")
			elif field[oldx][oldy].count(":") == 1:
				(oldfigure, oldcolor) = field[oldx][oldy].split(":")
				oldoptions = ""
		print "go with", oldfigure, "of color", oldcolor, "and options", oldoptions
		figure = ""
		color = ""
		if field[x][y] is not "":
			if field[x][y].count(":") == 2:
				(figure, color, options) = field[x][y].split(":")
			elif field[x][y].count(":") == 1:
				(figure, color) = field[x][y].split(":")
				options = ""
		print "to", figure, "of color", color

		if oldfigure == "":
			return proto.move_invalid
		if oldcolor == color:
			return proto.move_invalid

		rivercross = 0
		if oldy < 5 and y >= 5:
			rivercross = 1
		if oldy >= 5 and y < 5:
			rivercross = 1

		insidecastle = 0
		if (x >= 3 and x <= 5) and (oldx >= 3 and oldx <= 5):
			if (oldy >= 7 and oldy <= 9) and (oldy >= 7 and oldy <= 9):
				insidecastle = 1
			if (oldy >= 0 and oldy <= 2) and (oldy >= 0 and oldy <= 2):
				insidecastle = 1

		print "try with rivercross", rivercross, "and insidecastle", insidecastle
		print "figure", oldfigure, "oldx/y", oldx, oldy, "x/y", x, y

		if oldfigure == "pawn":
			if rivercross:
				# XXX: promote
				field[oldx][oldy] = oldfigure + ":" + oldcolor + ":" + oldoptions + ",promoted"
			if oldcolor == "white":
				if x == oldx and y == oldy + 1:
					ret = proto.move_ok
			if oldcolor == "black":
				if x == oldx and y == oldy - 1:
					ret = proto.move_ok
			if "promoted" in oldoptions:
				if y == oldy and x == oldx - 1:
					ret = proto.move_ok
				if y == oldy and x == oldx + 1:
					ret = proto.move_ok

		if oldfigure == "knight":
			# XXX can be blocked!
			if abs(oldy - y) == 2 and abs(oldx - x) == 1:
				ret = proto.move_ok
			if abs(oldy - y) == 1 and abs(oldx - x) == 2:
				ret = proto.move_ok

		if oldfigure == "bishop":
			# XXX: can be blocked!
			if not rivercross:
				if abs(oldx - x) == 2 and abs(oldy - y) == 2:
					ret = proto.move_ok

		if oldfigure == "bodyguard":
			if insidecastle:
				if abs(oldy - y) == 1 and abs(oldx - x) == 1:
					ret = proto.move_ok

		if oldfigure == "cannon":
			# XXX: jumps!
			if oldx == x or oldy == y:
				ret = proto.move_ok

		if oldfigure == "rock":
			# XXX: can be blocked!
			if oldx == x or oldy == y:
				ret = proto.move_ok

		if oldfigure == "king":
			if insidecastle:
				if abs(oldx - x) == 1 and y == oldy:
					ret = proto.move_ok
				if abs(oldy - y) == 1 and x == oldx:
					ret = proto.move_ok

		return ret

	def setup(self):
		field = self.field

		for i in range(10):
			field[i] = {}
			for j in range(11):
				field[i][j] = ''

		for i in range(0, 5):
			field[i * 2][3] = 'pawn:white'
			field[i * 2][6] = 'pawn:black'

		field[1][2] = 'cannon:white'
		field[7][2] = 'cannon:white'
		field[1][7] = 'cannon:black'
		field[7][7] = 'cannon:black'

		field[0][0] = 'rock:white'
		field[1][0] = 'knight:white'
		field[2][0] = 'bishop:white'
		field[3][0] = 'bodyguard:white'
		field[4][0] = 'king:white:foo'
		field[5][0] = 'bodyguard:white'
		field[6][0] = 'bishop:white'
		field[7][0] = 'knight:white'
		field[8][0] = 'rock:white'

		field[0][9] = 'rock:black'
		field[1][9] = 'knight:black'
		field[2][9] = 'bishop:black'
		field[3][9] = 'bodyguard:black'
		field[4][9] = 'king:black'
		field[5][9] = 'bodyguard:black'
		field[6][9] = 'bishop:black'
		field[7][9] = 'knight:black'
		field[8][9] = 'rock:black'

		self.field = field

