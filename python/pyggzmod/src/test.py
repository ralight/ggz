#!/usr/bin/env python
#
# Test game client for the GGZ Gaming Zone
# Written in Python

import ggzmod

def hook_state ():
	print "* state"

def hook_join (seat):
	print "* join: ", seat
	print "(Name: " + ggzmod.getPlayerName(seat) + ")"

def hook_leave (seat):
	print "* leave: ", seat
	print "(Name: " + ggzmod.getPlayerName(seat) + ")"

def hook_log ():
	print "* log"

def hook_data (seat):
	print "* data: ", seat
	print "(Name: " + ggzmod.getPlayerName(seat) + ")"

def hook_error (arg):
	print "* error"

print ">> register callbacks"
ggzmod.setHandler(ggzmod.EVENT_STATE, hook_state)

print ">> launch the game client"
ggzmod.connect()
ggzmod.autonetwork(0, -1)
#ggzmod.mainLoop()

print ">> done."

