#!/usr/bin/env python
#
# Test game client for the GGZ Gaming Zone
# Written in Python, using pyggzmod
# This is the variant for Pygame integration!

import sys
import pygame
#import pygame.ggzmod
import ggzmodpygame
pygame.ggzmod = ggzmodpygame
ggzmod = pygame.ggzmod.ggzmod

# Initialisation of pygame.ggzmod extension

print ">> launch the game client (pygame integration variant)"

pygame.init()

try:
	pygame.ggzmod.init()
except:
	print "Error: GGZ initialisation failed!"
	sys.exit()

# Double-checking: should return true

print "Is GGZ ready?",  pygame.ggzmod.get_init()

# Run Pygame event loop and check for GGZ events

print ">> loop..."

while 1:
	pygame.event.pump()
	event = pygame.event.wait()

	print "Pygame event arrived."
	print "Content:", event

	if event.type == pygame.ggzmod.GGZMODEVENT:
		print "It's a ggzmod event!"
		print "Type id", event.id
		print "=> Type name", pygame.ggzmod.events[event.id]

		if event.id == pygame.ggzmod.ggzmod.EVENT_STATE:
			print "* state event; old state:", pygame.ggzmod.states[event.oldstate]
			print " - new state is:", pygame.ggzmod.states[ggzmod.getState()]
		else:
			# proceed as in test.py
			pass

	elif event.type == pygame.ggzmod.GGZMODDATA:
		print "It's a game server data event!"
		c = pygame.ggzmod.channel
		# now, read data from socket via ggzmod.getFd() aka pygame.ggzmod.channel
		# this is the game specific protocol, here for Tic-Tac-Toe!
		data = c.recv(1000)
		print "=> read", len(data), "bytes"

print ">> done."

