# GGZBoard Network: Network base class for GGZBoard container
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import ggzmod
import socket

class NetworkBase:
	def __init__(self):
		self.sock = None
		self.gamefd = -1
		self.errorcode = 0
		self.inputallowed = 0
		self.inputthinking = 0

	def handle_network(self):
		ret = ggzmod.autonetwork()
		if ret:
			self.network()

	def network(self):
		print "ERROR: PURE VIRTUAL METHOD!"

	def handle_server(self, fd):
		ggzmod.setState(ggzmod.STATE_PLAYING)

		self.gamefd = fd
		self.init(self.gamefd)

	def init(self, fd):
		self.sock = socket.fromfd(fd, socket.AF_INET, socket.SOCK_STREAM)

	def connect(self):
		ggzmod.setHandler(ggzmod.EVENT_SERVER, self.handle_server)
		ret = ggzmod.connect()
		return ret

	def error(self):
		return self.errorcode

	def allowed(self):
		return self.inputallowed
	
	def thinking(self):
		return self.inputthinking

	def getbyte(self):
		opstr = self.sock.recv(4)
		if len(opstr) < 4:
			self.errorcode = 1
			return 0
		c1 = ord(opstr[0])
		c2 = ord(opstr[1])
		c3 = ord(opstr[2])
		c4 = ord(opstr[3])
		op = c1 * 256 * 256 * 256
		op += c2 * 256 * 256
		op += c3 * 256
		op += c4
		if socket.ntohl(op) == op:
			op = socket.ntohl(op)
		return op

	def getchar(self):
		opstr = self.sock.recv(1)
		if len(opstr) < 1:
			self.errorcode = 1
			return 0
		op = ord(opstr[0])
		return op

	def getstring(self):
		length = self.getbyte()
		opstr = ""
		if not self.errorcode:
			opstr = self.sock.recv(length)
			if len(opstr) < length:
				self.errorcode = 1
				return ""
		return opstr

	def sendbyte(self, byte):
		nbyte = byte
		if socket.htonl(nbyte) == nbyte:
			nbyte = socket.htonl(nbyte)
		c1 = (nbyte >> 24) & 0xFF
		c2 = (nbyte >> 16) & 0xFF
		c3 = (nbyte >> 8) & 0xFF
		c4 = (nbyte >> 0) & 0xFF
		s = chr(c1) + chr(c2) + chr(c3) + chr(c4)
		self.sock.send(s)

	def sendchar(self, char):
		self.sock.send(chr(char))

	def sendstring(self, str):
		self.sendbyte(len(str))
		self.sock.send(str)

