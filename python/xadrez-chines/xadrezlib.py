#!/usr/bin/env python
# Backend library for the Chinese Chess game
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random
import socket

class Network:
	def __init__(self):
		self.sock = None
		self.gamefd = -1

		#self.game = Game()

	def init(self, fd):
		self.sock = socket.fromfd(fd, socket.AF_INET, socket.SOCK_STREAM)

	def getbyte(self):
		opstr = self.sock.recv(4)
		op = ord(opstr[0]) * 256 * 256 * 256
		op += ord(opstr[1]) * 256 * 256
		op += ord(opstr[2]) * 256
		op += ord(opstr[3])
		return op

	def getchar(self):
		opstr = self.sock.recv(1)
		op = ord(opstr[0])
		return op

	def getstring(self):
		length = self.getbyte()
		opstr = self.sock.recv(length)
		return opstr

	def sendbyte(self, byte):
		nbyte = socket.htonl(byte)
		c1 = (nbyte >> 0) & 0xFF
		c2 = (nbyte >> 8) & 0xFF
		c3 = (nbyte >> 16) & 0xFF
		c4 = (nbyte >> 24) & 0xFF
		self.sock.send(chr(c1))
		self.sock.send(chr(c2))
		self.sock.send(chr(c3))
		self.sock.send(chr(c4))

	def sendchar(self, char):
		self.sock.send(chr(char))

	def sendstring(self, str):
		self.sendbyte(len(str))
		self.sock.send(str)

class Proto:
	def __init__(self):
		self.move_invalid = 101
		self.move_ok = 102

class Xadrez:
	def __init__(self):
		self.proto = Proto()
		self.field = {}
		self.setup()
		self.aicol = "white"
		self.win = ""
		self.aituple = ()

	def setaicolor(self, color):
		self.aicolor = color

	def aicolor(self):
		return self.aicol

	def winner(self):
		return self.win

	def ai(self):
		ret = self.proto.move_invalid
		tries = 0
		while ret == self.proto.move_invalid and tries < 1000:
			x = random.randint(0, 8)
			y = random.randint(0, 9)
			oldx = random.randint(0, 8)
			oldy = random.randint(0, 9)
			(figure, color, options) = self.components(self.field[oldx][oldy])
			if color == self.aicol:
				ret = self.validate(oldx, oldy, x, y)
			tries += 1
		if ret == self.proto.move_ok:
			self.aituple = (oldx, oldy, x, y)
		return ret

	def aimove(self):
		return self.aituple

	def components(self, fieldcell):
		if fieldcell.count(":") == 2:
			(figure, color, options) = fieldcell.split(":")
		elif fieldcell.count(":") == 1:
			(figure, color) = fieldcell.split(":")
			options = ""
		else:
			figure = ""
			color = ""
			options = ""
		return (figure, color, options)

	def validate(self, oldx, oldy, x, y):
		proto = self.proto
		field = self.field

		ret = proto.move_invalid

		(oldfigure, oldcolor, oldoptions) = self.components(field[oldx][oldy])
		print "go with", oldfigure, "of color", oldcolor, "and options", oldoptions
		(figure, color, options) = self.components(field[x][y])
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

		inbetween = 0
		if oldfigure in ("cannon", "rock", "bishop"):
			len = 0
			if abs(y - oldy) < abs(x - oldx):
				len = x - oldx
			else:
				len = y - oldy
			stepsize = 1
			#if oldfigure == "bishop":
			#	stepsize = 2
			for i in range(stepsize, abs(len), stepsize):
				xn = oldx + i * (x - oldx) / abs(len)
				yn = oldy + i * (y - oldy) / abs(len)
				print "try", xn, yn
				(tmpfigure, tmpcolor, tmpoptions) = self.components(field[xn][yn])
				if tmpfigure is not "":
					inbetween += 1

		if oldfigure in ("knight"):
			if abs(x - oldx) == 1 and y - oldy != 0:
				xn = oldx
				yn = oldy + (y - oldy) / abs(y - oldy)
			elif x - oldx != 0:
				xn = oldx + (x - oldx) / abs(x - oldx)
				yn = oldy
			else:
				xn = oldx
				yn = oldy
			print "try", xn, yn
			(tmpfigure, tmpcolor, tmpoptions) = self.components(field[xn][yn])
			if tmpfigure is not "":
				inbetween = 1
				
		if inbetween == 1:
			print "figure is blocked!"

		if oldfigure == "knight":
			if not inbetween:
				if abs(oldy - y) == 2 and abs(oldx - x) == 1:
					ret = proto.move_ok
				if abs(oldy - y) == 1 and abs(oldx - x) == 2:
					ret = proto.move_ok

		if oldfigure == "bishop":
			if not rivercross:
				if not inbetween:
					if abs(oldx - x) == 2 and abs(oldy - y) == 2:
						ret = proto.move_ok

		if oldfigure == "bodyguard":
			if insidecastle:
				if abs(oldy - y) == 1 and abs(oldx - x) == 1:
					ret = proto.move_ok

		if oldfigure == "cannon":
			if oldx == x or oldy == y:
				if figure == "":
					if not inbetween:
						ret = proto.move_ok
				else:
					if inbetween == 1:
						ret = proto.move_ok

		if oldfigure == "rock":
			if oldx == x or oldy == y:
				if not inbetween:
					ret = proto.move_ok

		if oldfigure == "king":
			if insidecastle:
				if abs(oldx - x) == 1 and y == oldy:
					ret = proto.move_ok
				if abs(oldy - y) == 1 and x == oldx:
					ret = proto.move_ok

		if ret == proto.move_ok:
			if figure == "king":
				self.win = color
			tmp = field[oldx][oldy]
			field[oldx][oldy] = ""
			field[x][y] = tmp

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

