#!/usr/bin/env python
#
# Test core client for the GGZ Gaming Zone
# Written in Python

import ggzcore
import inspect

def server_event(id, data):
	print "server event!"
	if id == 15:
		print "state change event!"
		print "state is now", s.get_state()
	elif id == 0:
		print "connected!"
	elif id == 1:
		print "connection error!"
		print "reason", data
	elif id == 2:
		print "negotiated!"
		ggzcore.server.login()
	elif id == 4:
		print "logged in!"
	elif id == 6:
		print "motd loaded!"
		print "motd is", data
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
s.set_logininfo(ggzcore.LOGIN_GUEST, "josef", "")
s.connect()

print ">> the current state"
state = s.get_state()
print state

counter = 299999
while counter is not 0:
	counter = counter - 1
	s.process()

print ">> done."

s.logout()
s.disconnect()

