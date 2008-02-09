#!/usr/bin/env python
#
# Launches a GGZ game server directly with two open seats
# Can be used as a test tool for game servers without the need for ggzd

import sys
import os
import socket
import signal

reset = "\x1b[39;49;00m"
red = "\x1b[31m"
green = "\x1b[32m"

def err(s):
	print red + s + reset

def out(s):
	print green + s + reset

def net_byte(byte):
	nbyte = byte
	if socket.htonl(nbyte) == nbyte:
		nbyte = socket.htonl(nbyte)
	c1 = (nbyte >> 24) & 0xFF
	c2 = (nbyte >> 16) & 0xFF
	c3 = (nbyte >> 8) & 0xFF
	c4 = (nbyte >> 0) & 0xFF
	s = chr(c1) + chr(c2) + chr(c3) + chr(c4)
	return s

def net_tobyte(s):
	if len(s) < 4:
		raise "unexpected read error"
		return 0
	c1 = ord(s[0])
	c2 = ord(s[1])
	c3 = ord(s[2])
	c4 = ord(s[3])
	op = c1 * 256 * 256 * 256
	op += c2 * 256 * 256
	op += c3 * 256
	op += c4
	if socket.ntohl(op) == op:
		op = socket.ntohl(op)
	return op

def initserver(socket):
	SEAT_OPEN = 1

	MSG_GAME_LAUNCH = 0
	MSG_GAME_SEAT = 1
	MSG_GAME_SPECTATOR_SEAT = 2
	MSG_GAME_RESEAT = 3
	RSP_GAME_STATE = 4

	MSG_LOG = 0
	REQ_GAME_STATE = 1
	REQ_NUM_SEATS = 2
	REQ_BOOT = 3
	REQ_BOT = 4
	REQ_OPEN = 5
	MSG_GAME_REPORT = 6
	MSG_SAVEGAME_REPORT = 7

	seats = net_byte(SEAT_OPEN) + net_byte(SEAT_OPEN)
	msg = net_byte(MSG_GAME_LAUNCH) + "testgame\0" + net_byte(2) + net_byte(0) + seats
	#print "MSG", msg
	#for m in msg:
	#	print "[" + str(ord(m)) + "]",
	#print
	socket.send(msg)

	while True:
		s = socket.recv(4)
		op = net_tobyte(s)

		if op == MSG_LOG:
			s = socket.recv(4)
			len = net_tobyte(s)
			s = socket.recv(len)
			out("Game server log: " + s)

def main():
	if len(sys.argv) != 2:
		err("Syntax error: needs game server executable")
		sys.exit(1)

	gameserver = sys.argv[1]

	(parentsock, childsock) = socket.socketpair()
	print parentsock
	print childsock

	pid = os.fork()
	if pid == 0:
		parentsock.close()
		# -- The following 3 lines would be necessary for ancient GGZ servers
		#if childsock.fileno() != 3:
		#	os.dup2(childsock.fileno(), 3)
		#	childsock.close()
		os.environ["GGZMODE"] = "1"
		os.environ["GGZSOCKET"] = str(childsock.fileno())
		try:
			os.execl(gameserver)
		except:
			err("Game server error: executable not found")
		os._exit(1)
	elif pid > 0:
		childsock.close()
	else:
		err("Error: fork failed")
		sys.exit(1)
	out("Launched " + gameserver + " with pid " + str(pid))

	try:
		initserver(parentsock)
	except NameError:
		raise
	except:
		err("Error: communication abortion, kill game server")
		parentsock.close()
		os.kill(pid, signal.SIGKILL)
		#raise

	try:
		ret = os.waitpid(pid, 0)
	except:
		err("Error: keyboard interrupt")
		sys.exit(1)

	if ret != 0:
		err("Error: game server execution failed")
		sys.exit(1)

	out("Finished execution successfully")

if __name__ == "__main__":
	try:
		main()
	except NameError:
		err("Error: program abortion due to syntax errors")
		raise

