#!/usr/bin/env python
#
# Pygame integration layer for pyggzmod,
# the GGZ Gaming Zone game client base library

import ggzmod
import pygame.event
import threading
import socket

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

events = {}
events[ggzmod.EVENT_STATE] = "state"
events[ggzmod.EVENT_SERVER] = "server"
events[ggzmod.EVENT_PLAYER] = "player"
events[ggzmod.EVENT_SEAT] = "seat"
events[ggzmod.EVENT_SPECTATOR] = "spectator"
events[ggzmod.EVENT_CHAT] = "chat"
events[ggzmod.EVENT_STATS] = "stats"
events[ggzmod.EVENT_INFO] = "info"
events[ggzmod.EVENT_ERROR] = "error"

# Event queue management

GGZMODEVENT = pygame.USEREVENT + 1
GGZMODDATA = pygame.USEREVENT + 2

_eventqueue = []

def addevent(type, dict):
	dict["id"] = type
	event = pygame.event.Event(GGZMODEVENT, dict)
	_eventqueue.append(event)

	# FIXME: totally **not** thread-safe
	pygame.event.post(event)

# Callback methods

def hook_state (oldstate):
	addevent(ggzmod.EVENT_STATE, {"oldstate": oldstate})

def hook_server (fd):
	addevent(ggzmod.EVENT_SERVER, {"fd": fd})

def hook_player (oldstatus, oldseat):
	addevent(ggzmod.EVENT_PLAYER, {"oldstatus": oldstatus, "oldseat": oldseat})

def hook_seat (oldseat):
	addevent(ggzmod.EVENT_SEAT, {"oldseat": oldseat})

def hook_spectator (oldseat):
	addevent(ggzmod.EVENT_SPECTATOR, {"oldseat": oldseat})

def hook_chat (data):
	addevent(ggzmod.EVENT_SPECTATOR, {"data": data})

def hook_stats ():
	addevent(ggzmod.EVENT_STATS, {})

def hook_info ():
	addevent(ggzmod.EVENT_INFO, {})

def hook_error (message):
	addevent(ggzmod.EVENT_ERROR, {"message": message})

# Callback setup

ggzmod.setHandler(ggzmod.EVENT_STATE, hook_state)
ggzmod.setHandler(ggzmod.EVENT_SERVER, hook_server)
ggzmod.setHandler(ggzmod.EVENT_PLAYER, hook_player)
ggzmod.setHandler(ggzmod.EVENT_SEAT, hook_seat)
ggzmod.setHandler(ggzmod.EVENT_SPECTATOR, hook_spectator)
ggzmod.setHandler(ggzmod.EVENT_CHAT, hook_chat)
ggzmod.setHandler(ggzmod.EVENT_STATS, hook_stats)
ggzmod.setHandler(ggzmod.EVENT_INFO, hook_info)
ggzmod.setHandler(ggzmod.EVENT_ERROR, hook_error)

# Pygame integration methods

_init_successful = False

def init():
	global _init_successful

	_init_successful = ggzmod.connect()
	if not _init_successful:
		raise "Pygame/ggzmod: could not connect to the core client"

	# Create dedicated thread
	t = threading.Thread(target = loop, name = "thread-ggzmod")
	t.start()

def get_init():
	return _init_successful

channel = None

def loop():
	global channel

	print "(ggzmod thread)"
	while 1:
		ret = ggzmod.autonetwork()
		if ret:
			if not channel:
				channel = socket.fromfd(ggzmod.getFd(), socket.AF_INET, socket.SOCK_STREAM)
			event = pygame.event.Event(GGZMODDATA, {})

			# FIXME: totally **not** thread-safe
			pygame.event.post(event)

