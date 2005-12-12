# Python library for GGZ common game client functionality
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
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

def rect(surface, color, x1, y1, w, h, bgcolor):
	surface.fill(bgcolor, ((x1, y1), (w, h)))
	surface.fill(color, ((x1, y1), (1, h)))
	surface.fill(color, ((x1, y1), (w, 1)))
	surface.fill(color, ((x1, y1 + h), (w, 1)))
	surface.fill(color, ((x1 + w, y1), (1, h)))

def menurender(surface):
	titlefont = pygame.font.SysFont("Vera Sans", 24)
	font = pygame.font.SysFont("Vera Sans", 12)

	title = titlefont.render(_("Player table"), 1, (255, 255, 255))
	surface.blit(title, (100, 50))

	for i in range(ggzmod.getNumSeats()):
		type = ggzmod.getSeatType(i)
		name = ggzmod.getSeatName(i)

		typename = ""
		if type == ggzmod.SEAT_PLAYER:
			typename = _("Occupied")
		elif type == ggzmod.SEAT_OPEN:
			typename = _("Empty")
		elif type == ggzmod.SEAT_BOT:
			typename = _("Bot")
		elif type == ggzmod.SEAT_RESERVED:
			typename = _("Reserved")
		elif type == ggzmod.SEAT_NONE:
			typename = "-"

		seatstr = font.render("#" + str(i), 1, (255, 255, 255))
		namestr = font.render(name, 1, (255, 255, 255))
		typestr = font.render(typename, 1, (255, 255, 255))

		surface.blit(seatstr, (100, i * 20 + 100))
		surface.blit(typestr, (150, i * 20 + 100))
		surface.blit(namestr, (250, i * 20 + 100))

	if ggzmod.getNumSeats() == 0:
		localstr = font.render(_("No players found. Looks like a local game."), 1, (255, 255, 255))

		surface.blit(localstr, (100, 20 + 100))
		

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

