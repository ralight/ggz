#!/usr/bin/env python
#
# Test core client for the GGZ Gaming Zone
# Written in Python

import ggzcore
import inspect

def minisleep(s):
	counter = 499999
	while counter is not 0:
		counter = counter - 1
		s.process()

def server_event(id, data):
	print "server event!"
	if id == ggzcore.SERVER_STATE_CHANGE:
		print "state change event!"
		print "state is now", s.get_state()
	elif id == ggzcore.SERVER_CONNECTED:
		print "connected!"
	elif id == ggzcore.SERVER_CONNECT_FAIL:
		print "connection error!"
		print "reason", data
	elif id == ggzcore.SERVER_NEGOTIATED:
		print "negotiated!"
		ggzcore.server.login()
	elif id == ggzcore.SERVER_LOGGED_IN:
		print "logged in!"
	elif id == ggzcore.SERVER_MOTD_LOADED:
		print "motd loaded!"
		print "motd is", data
	elif id == ggzcore.SERVER_ROOM_LIST:
		print "room list!"
		print data
		for room in data:
			print "room =", room
	elif id == ggzcore.SERVER_ENTERED:
		print "we are in the room!"
		print ggzcore.room.get_name()
		print ggzcore.room.get_desc()
	else:
		print "!! unknown event", id

def room_event():
	print "room event!"

print ">> this is ggzcore reloaded"
ggzcore.reload()
ggzcore.setHandler(ggzcore.EVENT_SERVER, server_event)
ggzcore.setHandler(ggzcore.EVENT_ROOM, room_event)

print ">> try to toy around with the server object"
s = ggzcore.server
print "** server is", s
print "** server dict is", inspect.getmembers(s)

s.set_hostinfo("localhost", 5688, 1)
s.set_logininfo(ggzcore.LOGIN_GUEST, "player", "")
s.connect()

print ">> the current state"
state = s.get_state()
print state

minisleep(s)

s.join_room('TicTacToe')

minisleep(s)

print ">> done."

s.logout()
s.disconnect()

