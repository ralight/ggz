#!/usr/bin/env python
# GGZBoard Go: Go game module for the GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import ggzmod
import socket
import os

from ggzboard_net import *

class NetworkInfo:
	def __init__(self):
		self.playernum = -1
		self.playerturn = -1
		self.playernames = None
		self.modified = 0
		self.playernum = -1

class Network(NetworkBase, NetworkInfo):
	def __init__(self):
		NetworkBase.__init__(self)
		NetworkInfo.__init__(self)

		self.movequeue = []

		self.sequence = 0
		self.peersequence = 0
		self.command_ok = 0
		self.command_deny = 1
		self.command_newgame = 2
		self.command_query = 3
		self.command_answer = 4
		self.command_move = 5
		self.command_takeback = 6
		self.command_extended = 7

		self.lastcommand = -1
		self.lastmove = None

		self.inputthinking = 1

	def network(self):
		print "network!"

		(len, chars) = self.getchars(256)
	
		state = "start"
		for c in chars:
			byte = ord(c)
			#print "#MSG#", byte

			if state == "start":
				if byte & 0x80:
					print "analyze error: start byte has high bit set"
				else:
					fixedbits = byte & 0xFC
					if fixedbits != 0:
						print "analyze error: sequence changed"
					else:
						clientbit = (byte & 2) >> 1
						serverbit = (byte & 1)
						print "* clientbit, serverbit:", clientbit, serverbit
						self.peersequence = serverbit
				state = "checksum"
			elif state == "checksum":
				if byte & 0x80:
					checksum = (byte & 0x7F)
					print "* checksum:", checksum
				else:
					print "analyze error: checksum byte is missing high bit"
				state = "data1"
			elif state == "data1":
				if byte & 0x80:
					basiccommand = (byte & 0x70) >> 4
					commandseparator = (byte & 8)
					commandoptions = (byte & 7)
				else:
					print "analyze error: first data byte is missing high bit"
				state = "data2"
			elif state == "data2":
				if byte & 0x80:
					tmpoptions = (byte & 0x7F)
					commandoptions = tmpoptions + (commandoptions << 7)
					print "* command, separator, options", basiccommand, commandseparator, commandoptions
					if basiccommand == self.command_ok:
						if commandoptions == 0x3FF:
							print "command: ok"
							if self.lastcommand == self.command_move:
								self.movequeue.append((None, self.lastmove))
						else:
							print "command error: ok data corrupted"
					elif basiccommand == self.command_deny:
						if commandoptions == 0x000:
							print "command: deny"
						else:
							print "command error: deny data corrupted"
					elif basiccommand == self.command_newgame:
						if commandoptions == 0x000:
							print "command: newgame"
							self.newgame()
						else:
							print "command error: newgame data corrupted"
					elif basiccommand == self.command_query:
						print "command: query"
						if commandoptions == 9: # board size
							self.sendcommand(self.command_answer, 19)
						elif commandoptions == 8: # handicap
							self.sendcommand(self.command_answer, 3)
						elif commandoptions == 11: # colour
							self.sendcommand(self.command_answer, 2) # 1=white, 2=black
						else:
							self.sendcommand(self.command_answer, 0)
					elif basiccommand == self.command_answer:
						print "command: answer"
					elif basiccommand == self.command_move:
						print "command: move"
						self.gotmove((commandoptions & 0x200) >> 9, commandoptions & 0x1FF)
						self.inputthinking = 0
					elif basiccommand == self.command_takeback:
						print "command: takeback"
					elif basiccommand == self.command_extended:
						print "command: extended"
					else:
						print "command error: unknown command"
				else:
					print "analyze error: second data byte is missing high bit"
				state = "start"

	def newgame(self):
		self.inputallowed = 1
		# FIXME: send command_ok, or command_query?
		self.sendcommand(self.command_ok, 0x3FF)

	def gotmove(self, color, move):
		x = move % 19
		y = move / 19
		topos = (x, 19 - y)
		print "<<<move", color, topos
		self.movequeue.append((None, topos))

	def sendcommand(self, basiccommand, commandoptions):
		print ">>>command", basiccommand
		self.lastcommand = basiccommand

		if basiccommand != self.command_ok:
			self.sequence += 1
		clientbit = self.sequence & 0x01
		serverbit = self.peersequence & 0x01
		c1 = (serverbit << 1) + clientbit  # sequence byte
		print ">>>sequences", serverbit, clientbit

		commandseparator = 0
		c3 = 0x80 + (basiccommand << 4) + (commandseparator << 3) + ((commandoptions & 0x380) >> 7) # data byte 1
		c4 = 0x80 + (commandoptions & 0x7F) # data byte 2

		checksum = c1 + c3 + c4
		print ">>>checksum", checksum, "from", c1, c3, c4
		c2 = 0x80 + (checksum & 0x7F) # checksum byte

		line = ""
		line += chr(c1)
		line += chr(c2)
		line += chr(c3)
		line += chr(c4)
		self.sendchars(line, len(line))

	def domove(self, frompos, topos):
		#(x, y) = frompos
		(x2, y2) = topos

		basiccommand = self.command_move
		p = 0 # black
		i = (19 - y2) * 19 + x2 # O1: 14 (lower right...)
		commandoptions = (p << 10) + i
		self.lastmove = (x2, y2)

		self.sendcommand(basiccommand, commandoptions)
		self.inputthinking = 1

	def netmove(self):
		if len(self.movequeue) == 0:
			return None
		else:
			move = self.movequeue.pop(0)
			return move

	def getchars(self, length):
		chars = os.read(self.sock.fileno(), 256)
		print "go::read", len(chars)
		return (len(chars), chars)

	def sendchars(self, chars, length):
		print "go::write", length
		os.write(self.sock.fileno(), chars)

ggzboardnet = Network()

