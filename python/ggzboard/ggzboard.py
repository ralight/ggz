#!/usr/bin/env python
# GGZBoard: Board game client container application
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import pygame
from pygame.locals import *
from Numeric import *
import sys
import time
import random
import os, pwd

import rsvgsdl

from module_hnefatafl import Game
#from module_reversi import Game
#from module_chess import Game
#from module_checkers import Game

class Conf:
	def __init__(self):
		self.alpha = 120
		self.background = "sunset.jpg"
		self.cellwidth = 50
		self.cellheight = 50
		self.cellspace = 0 #10
		self.marginwidth = 20
		self.marginheight = 100
		self.tilecolor = (100, 100, 100)
		self.darktilecolor = (50, 50, 50)
		self.resolution = (800, 600)
		self.fullscreen = 0

def rect(surface, color, x1, y1, w, h, bgcolor):
	global conf

	tmp = pygame.Surface((w, h))
	tmp.fill(bgcolor)
	tmp.set_alpha(conf.alpha)
	surface.blit(tmp, (x1, y1))
	surface.fill(color, ((x1, y1), (1, h)))
	surface.fill(color, ((x1, y1), (w, 1)))
	surface.fill(color, ((x1, y1 + h), (w, 1)))
	surface.fill(color, ((x1 + w, y1), (1, h)))

def svgsurface(filename, width, height):
	rawdata = rsvgsdl.render(filename, width, height)
	if rawdata:
		surface = pygame.image.fromstring(rawdata, (width, height), "RGBA")
	else:
		surface = pygame.Surface((width, height))
	return surface

def main():
	global conf

	conf = Conf()
	game = Game()

	pygame.init()
	pygame.display.set_caption("GGZBoard: " + game.name())
	screen = pygame.display.set_mode(conf.resolution, DOUBLEBUF)
	if conf.fullscreen:
		pygame.display.toggle_fullscreen()

	if conf.background:
		surface = pygame.image.load(conf.background)
	else:
		surface = pygame.Surface(conf.resolution)

	font = pygame.font.SysFont("vera", 48)
	img = font.render("GGZBoard: " + game.name(), 1, (255, 200, 0))
	surface.blit(img, (20, 30))

	pygame.event.clear()

	updatescreen = 1

	board = game.board

	oldx = -1
	oldy = -1
	aiturn = 0
	shift = 0
	inputallowed = 1

	width = (conf.cellwidth + conf.cellspace)
	height = (conf.cellheight + conf.cellspace)

	game.background = pygame.Surface((surface.get_width(), surface.get_height()))
	game.background.blit(surface, (0, 0))

	while 1:
		pygame.event.pump()
		event = pygame.event.poll()

		if event.type == KEYDOWN:
			key = event.key
			if key == K_ESCAPE or pygame.event.peek(QUIT):
				break

		if event.type == MOUSEMOTION:
			(posx, posy) = event.pos

		if event.type == MOUSEBUTTONDOWN and inputallowed:
			x = (posx - conf.marginwidth) % width
			y = (posy - conf.marginheight) % height
			if x < conf.cellwidth and y < conf.cellheight:
				x = ((posx - conf.marginwidth) / width)
				y = ((posy - conf.marginheight) / height)
				if x >= game.width or y >= game.height or x < 0 or y < 0:
					# ignore
					pass
				elif oldx == -1:
					if game.setonly:
						ret = game.trymove(None, (x, y))
						if ret:
							game.domove(None, (x, y))
							aiturn = 1
					else:
						oldx = x
						oldy = y
					updatescreen = 1
				else:
					frompos = (oldx, oldy)
					topos = (x, y)
					ret = game.trymove(frompos, topos)
					print "RET(trymove)", ret
					if ret:
						game.domove(frompos, topos)
						aiturn = 1

					updatescreen = 1

					oldx = -1
					oldy = -1

		if updatescreen:
			surface.blit(game.background, (0, 0))

			for j in range(0, game.height):
				for i in range(0, game.width):
					color = conf.tilecolor
					if game.swaptiles and (i + j) % 2:
						color = conf.darktilecolor
					if j == oldy and i == oldx:
						(r, g, b) = color
						color = (min(r + 50, 255), min(g + 50, 255), min(b + 50, 255))
					sx = i * width + conf.marginwidth
					sy = j * height + conf.marginheight
					rect(surface, (255, 255, 255), sx, sy, conf.cellwidth, conf.cellheight, color)
					if board[j][i]:
						img = svgsurface(game.figure(board[j][i]), conf.cellwidth, conf.cellheight)
						surface.blit(img, (sx, sy))

			screen.blit(surface, (0, 0))
			updatescreen = 0

		pygame.display.flip()

		if inputallowed and game.over():
			aiturn = 0
			inputallowed = 0

		if aiturn:
			aiturn = 0
			(ret, frompos, topos) = game.aimove()
			print "RET(find)", ret
			if ret:
				game.domove(frompos, topos)

			updatescreen = 1

			oldx = -1
			oldy = -1

if __name__ == "__main__":
	main()

