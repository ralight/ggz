#!/usr/bin/env python
#
# Test core client for the GGZ Gaming Zone
# Written in Python

import ggzcore

def server_event():
	print "server event!"

def room_event():
	print "room event!"

print ">> this is ggzcore reloaded"
ggzcore.reload()
ggzcore.setHandler(ggzcore.EVENT_SERVER, server_event)
ggzcore.setHandler(ggzcore.EVENT_ROOM, room_event)

print ">> try to toy around with the server object"
s = ggzcore.server
s.set_hostinfo("localhost", 5688, 1)
s.set_logininfo(ggzcore.LOGIN_GUEST, "josef", "")
s.connect()

print ">> the current state"
state = s.get_state()
print state

while 1:
	s.process()

print ">> done."

s.logout()
s.disconnect()

