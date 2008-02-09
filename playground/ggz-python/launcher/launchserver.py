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

def net_int(int):
	nint = int
	if socket.htonl(nint) == nint:
		nint = socket.htonl(nint)
	c1 = (nint >> 24) & 0xFF
	c2 = (nint >> 16) & 0xFF
	c3 = (nint >> 8) & 0xFF
	c4 = (nint >> 0) & 0xFF
	s = chr(c1) + chr(c2) + chr(c3) + chr(c4)
	return s

def net_string(str):
	s = net_int(len(str) + 1)
	s += str
	s += "\0"
	return s

def net_toint(s):
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

def reverse_dict(d):
	rev = {}
	for key in d:
		value = d[key]
		rev[value] = key
	return rev

class Protocol:
	SEAT_NONE = 0
	SEAT_OPEN = 1
	SEAT_BOT = 2
	SEAT_PLAYER = 3
	SEAT_RESERVED = 4
	SEAT_ABANDONED = 5

	TABLE_ERROR = -1
	TABLE_CREATED = 0
	TABLE_WAITING = 1
	TABLE_PLAYING = 2
	TABLE_DONE = 3

	CHAT_UNKNOWN = 0
	CHAT_NORMAL = 1
	CHAT_ANNOUNCE = 2
	CHAT_BEEP = 3
	CHAT_PERSONAL = 4
	CHAT_TABLE = 5

	MSG_GAME_LAUNCH = 0
	MSG_GAME_SEAT = 1 # FIXME: should be REQ_GAME_SEAT in protocol.h?
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

	tablenames = {}
	tablenames[TABLE_ERROR] = "error"
	tablenames[TABLE_CREATED] = "created"
	tablenames[TABLE_WAITING] = "waiting"
	tablenames[TABLE_PLAYING] = "playing"
	tablenames[TABLE_DONE] = "done"

	seattypes = {}
	seattypes[SEAT_NONE] = "none"
	seattypes[SEAT_OPEN] = "open"
	seattypes[SEAT_BOT] = "bot"
	seattypes[SEAT_PLAYER] = "player"
	seattypes[SEAT_RESERVED] = "reserved"
	seattypes[SEAT_ABANDONED] = "abandoned"

	tablenames_reverse = reverse_dict(tablenames)
	seattypes_reverse = reverse_dict(seattypes)

def initserver(sock, gamename, seats, seatnames):
	spectatorseats = []

	seatsmsg = net_int(len(seats))
	seatsmsg += net_int(len(spectatorseats))
	for seat in seats:
		seatsmsg += net_int(seat)
		if seat == Protocol.SEAT_RESERVED or seat == Protocol.SEAT_BOT:
			seatname = seatnames[seats.index(seat)]
			seatsmsg += net_string(seatname)

	msg = net_int(Protocol.MSG_GAME_LAUNCH) + net_string(gamename) + seatsmsg
	#print "MSG", msg
	#for m in msg:
	#	print "[" + str(ord(m)) + "]",
	#print
	sock.send(msg)

	# FIXME: this is a fake player
	#if seats[0] == Protocol.SEAT_PLAYER:
	(parentsock, childsock) = socket.socketpair()
	playermsg = net_int(Protocol.SEAT_PLAYER) + net_string("someplayer") + net_int(childsock.fileno())
	msg = net_int(Protocol.MSG_GAME_SEAT) + net_int(0) + playermsg
	sock.send(msg)

	while True:
		s = sock.recv(4)
		op = net_toint(s)

		if op == Protocol.MSG_LOG:
			s = sock.recv(4)
			strlen = net_toint(s)
			s = sock.recv(strlen)
			out("Game server log: " + s)
		elif op == Protocol.REQ_GAME_STATE:
			s = sock.recv(1)
			state = ord(s)

			if Protocol.tablenames.has_key(state):
				statename = Protocol.tablenames[state]
			else:
				statename = "???"
			out("Game server state: " + statename)

			msg = net_int(Protocol.RSP_GAME_STATE)
			sock.send(msg)
		elif op == Protocol.REQ_NUM_SEATS:
			err("FIXME: unhandled opcode")
			pass
		elif op == Protocol.REQ_BOOT:
			err("FIXME: unhandled opcode")
			pass
		elif op == Protocol.REQ_BOT:
			err("FIXME: unhandled opcode")
			pass
		elif op == Protocol.REQ_OPEN:
			err("FIXME: unhandled opcode")
			pass
		elif op == Protocol.MSG_GAME_REPORT:
			err("FIXME: unhandled opcode")
			pass
		elif op == Protocol.MSG_SAVEGAME_REPORT:
			err("FIXME: unhandled opcode")
			pass
		else:
			err("Game server protocol error: unknown opcode")
			raise "unexpected opcode error"

def main():
	if len(sys.argv) != 2:
		err("Syntax error: needs game server executable")
		sys.exit(1)

	gameserver = sys.argv[1]

	try:
		gamename = raw_input("Game name: ")
		numseats = raw_input("Number of seats: ")
		seats = []
		seatnames = []
		print "Seat types: open|bot|reserved"
		for i in range(int(numseats)):
			seattype = raw_input("Type of seat " + str(i) + ": ")
			if seattype in ["open", "bot", "reserved"]:
				seat = Protocol.seattypes_reverse[seattype]
				seats.append(seat)
				seatname = ""
				if seat == Protocol.SEAT_RESERVED or seat == Protocol.SEAT_BOT:
					seatname = raw_input("Name of player/bot in seat " + str(i) + ": ")
				seatnames.append(seatname)
			else:
				raise "unexpected value for seat"
	except:
		err("Syntax error: invalid options")
		sys.exit(1)

	(parentsock, childsock) = socket.socketpair()
	#print parentsock
	#print childsock

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
		initserver(parentsock, gamename, seats, seatnames)
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

