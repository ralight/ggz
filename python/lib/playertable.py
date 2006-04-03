# Python library for GGZ common game client functionality
# Copyright (C) 2004 - 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import pygame
from pygame.locals import *
import sys
import time
import os, pwd
import gettext

import ggzmod

""" Internationalization """

gettext.install("ggzpython", "/usr/local/share/locale", 1)

#def rect(surface, color, x1, y1, w, h, bgcolor):
#	surface.fill(bgcolor, ((x1, y1), (w, h)))
#	surface.fill(color, ((x1, y1), (1, h)))
#	surface.fill(color, ((x1, y1), (w, 1)))
#	surface.fill(color, ((x1, y1 + h), (w, 1)))
#	surface.fill(color, ((x1 + w, y1), (1, h)))

def menurender(surface):
	titlefont = pygame.font.SysFont("Vera Sans", 24)
	font = pygame.font.SysFont("Vera Sans", 12)

	if ggzmod.getNumSeats() == 0:
		excuse = _("No players found. Looks like a local game.")
		localstr = font.render(excuse, 1, (255, 255, 255))
		surface.blit(localstr, (100, 110))
		return

	offset = 0

	title = titlefont.render(_("Player table"), 1, (255, 255, 255))
	surface.blit(title, (100, 70))

	font.set_italic(1)
	seatstr = font.render(_("Seat number"), 1, (255, 255, 255))
	typestr = font.render(_("Type"), 1, (255, 255, 255))
	namestr = font.render(_("Name"), 1, (255, 255, 255))
	font.set_italic(0)

	surface.blit(seatstr, (100, 110))
	surface.blit(typestr, (200, 110))
	surface.blit(namestr, (300, 110))

	for i in range(ggzmod.getNumSeats()):
		seat = ggzmod.getSeat(i)
		if not seat:
			continue
		(num, type, name) = seat

		typename = ""
		if type == ggzmod.SEAT_PLAYER:
			typename = _("Occupied")
		elif type == ggzmod.SEAT_OPEN:
			typename = _("Empty")
		elif type == ggzmod.SEAT_BOT:
			typename = _("Bot")
		elif type == ggzmod.SEAT_RESERVED:
			typename = _("Reserved")
		elif type == ggzmod.SEAT_ABANDONED:
			typename = _("Abandoned")
		elif type == ggzmod.SEAT_NONE:
			typename = "-"

		color = (255, 255, 255)
		player = ggzmod.getPlayer()
		if player:
			(playername, isspectator, num) = player
			if not isspectator and num == i:
				color = (255, 255, 0)

		seatstr = font.render("#" + str(i), 1, color)
		typestr = font.render(typename, 1, color)
		namestr = font.render(name, 1, color)

		surface.blit(seatstr, (100, i * 20 + 130))
		surface.blit(typestr, (200, i * 20 + 130))
		surface.blit(namestr, (300, i * 20 + 130))

	offset = ggzmod.getNumSeats() * 20 + 130 + 20 - 70
	infooffset = 0

	title = titlefont.render(_("Spectator table"), 1, (255, 255, 255))
	surface.blit(title, (100, 70 + offset))

	font.set_italic(1)
	seatstr = font.render(_("Spectator number"), 1, (255, 255, 255))
	namestr = font.render(_("Name"), 1, (255, 255, 255))
	excusestr = font.render(_("No spectators present."), 1, (255, 255, 255))
	font.set_italic(0)

	if ggzmod.getNumSpectators() == 0:
		surface.blit(excusestr, (100, 110 + offset))
	else:
		surface.blit(seatstr, (100, 110 + offset))
		surface.blit(namestr, (300, 110 + offset))

	for i in range(ggzmod.getNumSpectators()):
		spectator = ggzmod.getSpectator(i)
		if not spectator:
			continue
		(num, name) = spectator

		color = (255, 255, 255)
		player = ggzmod.getPlayer()
		if player:
			(name, isspectator, num) = player
			if not isspectator and num == i:
				color = (255, 255, 0)

		seatstr = font.render("#" + str(i), 1, color)
		namestr = font.render(name, 1, color)

		surface.blit(seatstr, (100, i * 20 + 130 + offset))
		surface.blit(namestr, (300, i * 20 + 130 + offset))

	infooffset = ggzmod.getNumSpectators() * 20 + 130 + 20 + offset

	title = titlefont.render(_("Game information"), 1, (255, 255, 255))
	surface.blit(title, (100, 0 + infooffset))

	state = _("(unknown)")
	xstate = ggzmod.getState()
	if xstate == ggzmod.STATE_CREATED:
		state = _("Game was launched")
	elif xstate == ggzmod.STATE_CONNECTED:
		state = _("Game is connected")
	elif xstate == ggzmod.STATE_WAITING:
		state = _("Waiting for players")
	elif xstate == ggzmod.STATE_PLAYING:
		state = _("Running game")
	elif xstate == ggzmod.STATE_DONE:
		state = _("Game is finished")

	font.set_italic(1)
	statecaptionstr = font.render(_("Game state:"), 1, (255, 255, 255))
	font.set_italic(0)
	statestr = font.render(state, 1, (255, 255, 255))

	surface.blit(statecaptionstr, (100, 40 + infooffset))
	surface.blit(statestr, (100, 60 + infooffset))

def show():
	""" Prepare Playerlist """

	screen = pygame.display.get_surface()

	screencopy = pygame.Surface((screen.get_width(), screen.get_height()))
	screencopy.blit(screen, (0, 0))

	alpha = 1
	if alpha:
		dark = pygame.Surface((screen.get_width(), screen.get_height()))
		dark.fill((0, 0, 0))
		dark.set_alpha(150)
		screen.blit(dark, (0, 0))

	surface = pygame.Surface((800, 600))
	surface.fill((0, 0, 100))

	pygame.event.clear()

	updatescreen = 1

	while 1:
		""" Control """

		pygame.event.pump()
		event = pygame.event.poll()
		if event.type == KEYDOWN:
			key = event.key
			if key == K_ESCAPE or pygame.event.peek(QUIT):
				break

		""" Logics """

		""" Drawing """

		if updatescreen:
			menurender(surface)
			xpos = (screen.get_width() - 800) / 2
			ypos = (screen.get_height() - 600) / 2
			screen.blit(surface, (xpos, ypos))
			updatescreen = 0
		pygame.display.flip()

	screen.blit(screencopy, (0, 0))
	pygame.display.flip()

