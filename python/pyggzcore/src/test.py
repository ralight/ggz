#!/usr/bin/env python
#
# Test core client for the GGZ Gaming Zone
# Written in Python

import ggzcoresimple
import inspect
import sys

def minisleep(s):
	counter = 499999
	while counter is not 0:
		counter = counter - 1
		s.process()

def server_event(id, data):
	print "server event!"
	if id == ggzcoresimple.SERVER_STATE_CHANGE:
		print "state change event!"
		print "state is now", s.get_state()
	elif id == ggzcoresimple.SERVER_CONNECTED:
		print "connected!"
	elif id == ggzcoresimple.SERVER_CONNECT_FAIL:
		print "connection error!"
		print "reason", data
		sys.exit()
	elif id == ggzcoresimple.SERVER_NEGOTIATED:
		print "negotiated!"
		ggzcoresimple.server.login()
	elif id == ggzcoresimple.SERVER_LOGGED_IN:
		print "logged in!"
	elif id == ggzcoresimple.SERVER_MOTD_LOADED:
		print "motd loaded!"
		print "motd is", data
	elif id == ggzcoresimple.SERVER_ROOM_LIST:
		print "room list!"
		print data
		for room in data:
			print "room =", room
	elif id == ggzcoresimple.SERVER_ENTERED:
		print "we are in the room!"
		print ggzcoresimple.room.get_name()
		print ggzcoresimple.room.get_desc()
	else:
		print "!! unknown server event", id

def room_event(id, data):
	print "room event!"
	if id == ggzcoresimple.ROOM_CHAT_EVENT:
		print "chat!"
		print "data:", data
	elif id == ggzcoresimple.ROOM_PLAYER_LIST:
		print "player list!"
		print data
		for player in data:
			print "player =", player
	else:
		print "!! unknown room event", id

print ">> this is ggzcoresimple reloaded"
#ggzcoresimple.reload()
ggzcoresimple.setHandler(ggzcoresimple.EVENT_SERVER, server_event)
ggzcoresimple.setHandler(ggzcoresimple.EVENT_ROOM, room_event)

print ">> try to toy around with the server object"
s = ggzcoresimple.server
print "** server is", s
print "** server dict is", inspect.getmembers(s)

s.set_hostinfo("localhost", 5688, 1)
s.set_logininfo(ggzcoresimple.LOGIN_GUEST, "player", "")
s.connect()

print ">> the current state"
state = s.get_state()
print state

minisleep(s)
minisleep(s)

print ">> go to room TTT"

s.join_room("TicTacToe")

minisleep(s)
minisleep(s)

print ">> go chat"

r = ggzcoresimple.room
r.chat(ggzcoresimple.CHAT_NORMAL, "", "fuuubaaar")

minisleep(s)

print ">> go play"

r.play("test game", 0)

minisleep(s)

print ">> go logout"

minisleep(s)
s.logout()

print ">> logout done. go disconnect"

s.disconnect()

