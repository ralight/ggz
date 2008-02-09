#!/usr/bin/env python
#
# Launches a GGZ game server directly with any table configuration
# Can be used as a test tool for game servers without the need for ggzd

import sys
import os
import socket
import signal
import ancillary
import ConfigParser
import select

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

class ClientProtocol:
	MSG_GAME_STATE = 0
	REQ_STAND = 1
	REQ_SIT = 2
	REQ_BOOT = 3
	REQ_OPEN = 4
	REQ_CHAT = 5
	REQ_INFO = 6
	REQ_RANKINGS = 7

	MSG_GAME_LAUNCH = 0
	MSG_GAME_SERVER = 1
	MSG_GAME_SERVER_FD = 2
	MSG_GAME_PLAYER = 3
	MSG_GAME_SEAT = 4
	MSG_GAME_SPECTATOR_SEAT = 5
	MSG_GAME_CHAT = 6
	MSG_GAME_STATS = 7
	MSG_GAME_INFO = 8
	MSG_GAME_RANKINGS = 9

	STATE_CREATED = 0
	STATE_CONNECTED = 1
	STATE_WAITING = 2
	STATE_PLAYING = 3
	STATE_DONE = 4

	statenames = {}
	statenames[STATE_CREATED] = "created"
	statenames[STATE_CONNECTED] = "connected"
	statenames[STATE_WAITING] = "waiting"
	statenames[STATE_PLAYING] = "playing"
	statenames[STATE_DONE] = "done"

def initclient(clientsock, serversock, parentsock, childsock, clientexecutable):
	# clientsock/serversock: game client<->game server
	# parentsock/childsock: launchserver.py<->game client

	pid = os.fork()
	if pid == 0:
		serversock.close()
		parentsock.close()
		os.environ["GGZMODE"] = "true"
		os.environ["GGZSOCKET"] = str(childsock.fileno())
		try:
			os.execl(clientexecutable)
		except:
			err("Game client error: executable not found")
		os._exit(1)
	elif pid > 0:
		childsock.close()
		# FIXME: read game client messages from parentsock

		msg = net_int(ClientProtocol.MSG_GAME_LAUNCH)
		parentsock.send(msg)

		msg = net_int(ClientProtocol.MSG_GAME_SERVER_FD)
		parentsock.send(msg)
		ancillary.write_fd(parentsock.fileno(), clientsock.fileno())
		###clientsock.close()
	else:
		err("Error: fork failed")
		return None
	out("Launched " + clientexecutable + " with pid " + str(pid))
	return pid

def initserver(sock, gamename, seats, seatnames, clientexecutable):
	pids = []
	csocks = []
	poller = select.poll()

	spectatorseats = []

	seatsmsg = net_int(len(seats))
	seatsmsg += net_int(len(spectatorseats))
	for i in range(len(seats)):
		seat = seats[i]
		seatsmsg += net_int(seat)
		if seat == Protocol.SEAT_RESERVED or seat == Protocol.SEAT_BOT:
			seatname = seatnames[i]
			seatsmsg += net_string(seatname)

	msg = net_int(Protocol.MSG_GAME_LAUNCH) + net_string(gamename) + seatsmsg
	#print "MSG", msg
	#for m in msg:
	#	print "[" + str(ord(m)) + "]",
	#print
	sock.send(msg)

	poller.register(sock.fileno(), select.POLLIN)

	# FIXME: this is a fake player
	if seats[0] == Protocol.SEAT_OPEN:
		out("Put a human into the first seat")
		(clientsock, serversock) = socket.socketpair()
		(parentsock, childsock) = socket.socketpair()
		playermsg = net_int(Protocol.SEAT_PLAYER) + net_string("someplayer")
		msg = net_int(Protocol.MSG_GAME_SEAT) + net_int(0) + playermsg
		sock.send(msg)
		ancillary.write_fd(sock.fileno(), serversock.fileno())
		###serversock.close()

		if clientexecutable:
			pid = initclient(clientsock, serversock, parentsock, childsock, clientexecutable)
			pids.append(pid)
			csocks.append(parentsock)
			poller.register(parentsock.fileno(), select.POLLIN)
		else:
			err("No game client executable known, forcing server-only mode")
	else:
		# FIXME: this is a bug in ggzdmod, if the first seat is already a bot we
		# shouldn't have to tell ggzdmod-game again!
		out("Put a bot into the first seat")
		playermsg = net_int(Protocol.SEAT_BOT) + net_string("somebot")
		msg = net_int(Protocol.MSG_GAME_SEAT) + net_int(0) + playermsg
		sock.send(msg)

	while True:
		fdtuples = poller.poll()
		for fdtuple in fdtuples:
			(fd, event) = fdtuple
			if fd == sock.fileno():
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
					err("FIXME: unhandled opcode numseats")
				elif op == Protocol.REQ_BOOT:
					err("FIXME: unhandled opcode reqboot")
				elif op == Protocol.REQ_BOT:
					err("FIXME: unhandled opcode reqbot")
				elif op == Protocol.REQ_OPEN:
					err("FIXME: unhandled opcode reqopen")
				elif op == Protocol.MSG_GAME_REPORT:
					s = sock.recv(4)
					numplayers = net_toint(s)
					out("Game server result for " + str(numplayers) + " players")
					for i in range(numplayers):
						s = sock.recv(4)
						strlen = net_toint(s)
						playername = sock.recv(strlen)
						s = sock.recv(4)
						playertype = net_toint(s)
						s = sock.recv(4)
						teamnumber = net_toint(s)
						s = sock.recv(4)
						result = net_toint(s)
						s = sock.recv(4)
						score = net_toint(s)
						out("- #" + str(i) + " Player " + playername + ": " + str(score))
				elif op == Protocol.MSG_SAVEGAME_REPORT:
					err("FIXME: unhandled opcode savegamereport")
				else:
					err("Game server protocol error: unknown opcode " + str(op))
					raise "unexpected opcode error"
			else:
				client = None
				for csock in csocks:
					if fd == csock.fileno():
						client = csock
						break
				if not client:
					continue

				s = client.recv(4)
				op = net_toint(s)

				if op == ClientProtocol.MSG_GAME_STATE:
					s = client.recv(1)
					state = ord(s)

					if ClientProtocol.statenames.has_key(state):
						statename = ClientProtocol.statenames[state]
					else:
						statename = "???"
					out("Game client state: " + statename)
				elif op == ClientProtocol.REQ_STAND:
					err("FIXME: unhandled client opcode reqstand")
				elif op == ClientProtocol.REQ_SIT:
					err("FIXME: unhandled client opcode reqsit")
				elif op == ClientProtocol.REQ_BOOT:
					err("FIXME: unhandled client opcode reqboot")
				elif op == ClientProtocol.REQ_OPEN:
					err("FIXME: unhandled client opcode reqopen")
				elif op == ClientProtocol.REQ_CHAT:
					err("FIXME: unhandled client opcode reqchat")
				elif op == ClientProtocol.REQ_INFO:
					err("FIXME: unhandled client opcode reqinfo")
				elif op == ClientProtocol.REQ_RANKINGS:
					err("FIXME: unhandled client opcode reqrankings")
				else:
					err("Game client protocol error: unknown opcode " + str(op))
					raise "unexpected opcode error"

	return pids

def main():
	if len(sys.argv) != 2 and len(sys.argv) != 3:
		err("Syntax error: needs game server executable")
		sys.exit(1)

	gameserver = sys.argv[1]
	clientexecutable = None
	if len(sys.argv) == 3:
		clientexecutable = sys.argv[2]

	gamename = None
	numseats = None
	seats = []
	seatnames = []

	try:
		ini = ConfigParser.SafeConfigParser()
		files = ini.read(os.path.expanduser('~/.ggz/launchserver.rc'))
		if len(files) == 0:
			raise "config file not found"
		if ini.has_section(gameserver):
			gamename = ini.get(gameserver, "name")
			numseats = ini.getint(gameserver, "seats")
			for i in range(numseats):
				try:
					seattype = ini.get(gameserver, "seat[" + str(i) + "]")
				except:
					seattype = None
				try:
					seatname = ini.get(gameserver, "seatname[" + str(i) + "]")
				except:
					seatname = None
				if seattype in ["open", "bot", "reserved"]:
					seat = Protocol.seattypes_reverse[seattype]
				else:
					seat = None
				seats.append(seat)
				seatnames.append(seatname)
	except:
		out("Notice: no config file loaded, querying all parameters")

	try:
		if not gamename:
			gamename = raw_input("Game name: ")
		if not numseats:
			numseats = raw_input("Number of seats: ")
		print "Seat types: open|bot|reserved"
		for i in range(int(numseats)):
			if len(seats) != numseats or seats[i] == None:
				seattype = raw_input("Type of seat " + str(i) + ": ")
				if seattype in ["open", "bot", "reserved"]:
					seat = Protocol.seattypes_reverse[seattype]
				else:
					raise "unexpected value for seat"
				if len(seats) != numseats:
					seats.append(seat)
				else:
					seats[i] = seat
			else:
				seat = seats[i]
			nameneeded = (seat == Protocol.SEAT_RESERVED or seat == Protocol.SEAT_BOT)
			if len(seatnames) != numseats or (seatnames[i] == None and nameneeded):
				seatname = None
				if nameneeded:
					seatname = raw_input("Name of player/bot in seat " + str(i) + ": ")
				if len(seatnames) != numseats:
					seatnames.append(seatname)
				else:
					seatnames[i] = seatname
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
		pids = initserver(parentsock, gamename, seats, seatnames, clientexecutable)
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

	for cpid in pids:
		try:
			ret = os.waitpid(cpid, 0)
		except:
			err("Error: keyboard interrupt")

		if ret != 0:
			err("Error: game client execution failed")
		else:
			out("Finished game client " + str(cpid))

	out("Finished game server execution successfully")

if __name__ == "__main__":
	try:
		main()
	except NameError:
		err("Error: program abortion due to syntax errors")
		raise

