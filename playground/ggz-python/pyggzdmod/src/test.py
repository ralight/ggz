#!/usr/bin/env python
#
# Test game server for the GGZ Gaming Zone
# Written in Python

import ggzdmod;

def hook_state ():
	print "* state"

def hook_join (seat):
	print "* join: ", seat
	print "(Name: " + ggzdmod.getPlayerName(seat) + ")"

def hook_leave (seat):
	print "* leave: ", seat
	print "(Name: " + ggzdmod.getPlayerName(seat) + ")"

def hook_log ():
	print "* log"

def hook_data (seat):
	print "* data: ", seat
	print "(Name: " + ggzdmod.getPlayerName(seat) + ")"

def hook_error (arg):
	print "* error"

print ">> register callbacks"
ggzdmod.setHandler(ggzdmod.EVENT_STATE, hook_state)
ggzdmod.setHandler(ggzdmod.EVENT_JOIN, hook_join)
ggzdmod.setHandler(ggzdmod.EVENT_LEAVE, hook_leave)
ggzdmod.setHandler(ggzdmod.EVENT_LOG, hook_log)
ggzdmod.setHandler(ggzdmod.EVENT_DATA, hook_data)
ggzdmod.setHandler(ggzdmod.EVENT_ERROR, hook_error)

print ">> launch the game server"
ggzdmod.connect()
ggzdmod.mainLoop()

print ">> done."

