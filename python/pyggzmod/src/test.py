#!/usr/bin/env python
#
# Test game client for the GGZ Gaming Zone
# Written in Python, using pyggzdmod

import ggzmod

# Debugging tables

states = {}
states[ggzmod.STATE_CREATED] = "created"
states[ggzmod.STATE_CONNECTED] = "connected"
states[ggzmod.STATE_WAITING] = "waiting"
states[ggzmod.STATE_PLAYING] = "playing"
states[ggzmod.STATE_DONE] = "done"

types = {}
types[ggzmod.SEAT_NONE] = "none"
types[ggzmod.SEAT_OPEN] = "open"
types[ggzmod.SEAT_BOT] = "bot"
types[ggzmod.SEAT_PLAYER] = "player"
types[ggzmod.SEAT_RESERVED] = "reserved"
types[ggzmod.SEAT_ABANDONED] = "abandoned"

# Callback methods

def hook_state (oldstate):
	print "* state event; old state:", states[oldstate]
	print " - new state is:", states[ggzmod.getState()]

def hook_server (fd):
	print "* server event; fd:", fd

def hook_player (oldstatus, oldseat):
	print "* player event; old spectator status:", oldstatus, ", old seat:", oldseat
	(name, isspectator, num) = ggzmod.getPlayer()
	print " - {name:", name, ", currently spectating:", isspectator,
	print ", seat number:", num, "}"

def hook_seat (oldseat):
	print "* seat event; old seat:", oldseat
	print " - there are now", ggzmod.getNumSeats(), "seats in total"
	(num, type, name) = ggzmod.getSeat(oldseat)
	print " - {num:", num, ", type:", types[type], ", name:", name, "}"

def hook_spectator (oldseat):
	print "* spectator event; old spectator seat:", oldseat
	print " - there are now", ggzmod.getNumSpectators(), "spectators in total"
	(num, name) = ggzmod.getSpectator(oldseat)
	print " - {num:", num, ", name:", name, "}"

def hook_chat (data):
	(sender, message) = data
	print "* chat event; sender:", sender, ", message:", message

def hook_stats ():
	print "* stats event"
	for i in range(ggzmod.getNumSeats()):
		record = ggzmod.getRecord(i)
		rating = ggzmod.getRating(i)
		ranking = ggzmod.getRanking(i)
		highscore = ggzmod.getHighscore(i)
		print " - seat", i, ": {record:", record, ", rating:", rating,
		print ", ranking:", ranking, ", highscore:", highscore, "}"

def hook_info ():
	print "* info event"
	for i in range(ggzmod.getNumSeats()):
		info = ggzmod.getInfo(i)
		if info:
			(num, realname, photo, host) = info
			print " - seat", i, ": {realname:", realname, ", photo:", photo,
			print ", host:", host, "}"
		else:
			print " - seat", i, ": (none)"

def hook_error (message):
	global running

	print "* error event; message:", message
	running = 0

print ">> register callbacks"
ggzmod.setHandler(ggzmod.EVENT_STATE, hook_state)
ggzmod.setHandler(ggzmod.EVENT_SERVER, hook_server)
ggzmod.setHandler(ggzmod.EVENT_PLAYER, hook_player)
ggzmod.setHandler(ggzmod.EVENT_SEAT, hook_seat)
ggzmod.setHandler(ggzmod.EVENT_SPECTATOR, hook_spectator)
ggzmod.setHandler(ggzmod.EVENT_CHAT, hook_chat)
ggzmod.setHandler(ggzmod.EVENT_STATS, hook_stats)
ggzmod.setHandler(ggzmod.EVENT_INFO, hook_info)
ggzmod.setHandler(ggzmod.EVENT_ERROR, hook_error)

print ">> launch the game client"
ret = ggzmod.connect()
if ret:
	print ">> loop..."
	running = 1
	requested = 0
	while running:
		ret = ggzmod.autonetwork()
		if ret:
			# data from game server could be read here :)
			# but instead, we use the chance to get some infos
			if not requested:
				print "# now, request player info for all"
				ggzmod.requestInfo(-1)
				requested = 1
			pass
else:
	print "!! could not connect to the core client"

print ">> done."

