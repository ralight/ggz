#!/usr/local/bin/python
#
# Test game server for the GGZ Gaming Zone
# Written in Python

import ggzdmod;

def hook_launch ():
	print "* launch"

def hook_join (seat):
	print "* join: ", seat
	print "(Name: " + ggzdmod.getPlayerName(seat) + ")"

def hook_leave (seat):
	print "* leave: ", seat
	print "(Name: " + ggzdmod.getPlayerName(seat) + ")"

def hook_quit ():
	print "* quit"

def hook_player (seat):
	print "* player: ", seat
	print "(Name: " + ggzdmod.getPlayerName(seat) + ")"

print ">> register callbacks"
ggzdmod.setHandler(ggzdmod.EVENT_LAUNCH, hook_launch)
ggzdmod.setHandler(ggzdmod.EVENT_JOIN, hook_join)
ggzdmod.setHandler(ggzdmod.EVENT_LEAVE, hook_leave)
ggzdmod.setHandler(ggzdmod.EVENT_QUIT, hook_quit)
ggzdmod.setHandler(ggzdmod.EVENT_PLAYER, hook_player)

print ">> launch the game server"
ggzdmod.connect()
ggzdmod.mainLoop()

print ">> done."

