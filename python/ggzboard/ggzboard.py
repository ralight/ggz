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
import imp
import re

import rsvgsdl

#from module_hnefatafl import Game
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

class GGZBoardUI:
	def __init__(self):
		self.surface = None
		self.font = None
		self.smallfone = None
		self.screen = None

class GGZBoard:
	def __init__(self):
		self.conf = None
		self.game = None
		self.modulelist = []
		self.modulefilelist = []
		self.ui = None

	def rect(self, surface, color, x1, y1, w, h, bgcolor):
		tmp = pygame.Surface((w, h))
		tmp.fill(bgcolor)
		tmp.set_alpha(self.conf.alpha)
		surface.blit(tmp, (x1, y1))
		surface.fill(color, ((x1, y1), (1, h)))
		surface.fill(color, ((x1, y1), (w, 1)))
		surface.fill(color, ((x1, y1 + h), (w, 1)))
		surface.fill(color, ((x1 + w, y1), (1, h)))

	def svgsurface(self, filename, width, height):
		rawdata = rsvgsdl.render(filename, width, height)
		if rawdata:
			surface = pygame.image.fromstring(rawdata, (width, height), "RGBA")
		else:
			surface = pygame.Surface((width, height))
		return surface

	def walker(self, arg, dirname, fnames):
		ex = re.compile("^module_(\S+).py$")
		for file in fnames:
			m = ex.match(file)
			if m:
				self.modulefilelist.append(file)
				self.modulelist.append(m.group(1))

	def load(self, gamename):
		os.path.walk(".", self.walker, None)

		if gamename:
			self.loadgame(gamename)

	def loadgame(self, gamename):
		(fileobj, filename, desc) = imp.find_module("module_" + gamename, ["."])
		mod = imp.load_module("ggzboardgame", fileobj, filename, desc)
		fileobj.close()
		self.game = mod.ggzboardgame

	def main(self, ggzmode):
		self.conf = Conf()
		self.ui = GGZBoardUI()

		pygame.init()
		pygame.display.set_caption("GGZBoard")

		self.ui.screen = pygame.display.set_mode(self.conf.resolution, DOUBLEBUF)
		if self.conf.fullscreen:
			pygame.display.toggle_fullscreen()

		if self.conf.background:
			self.ui.surface = pygame.image.load(self.conf.background)
		else:
			self.ui.surface = pygame.Surface(self.conf.resolution)

		self.ui.font = pygame.font.SysFont("vera", 48)
		self.ui.smallfont = pygame.font.SysFont("vera", 24)

		if not self.game:
			self.intro()
			if self.conf.background:
				self.ui.surface = pygame.image.load(self.conf.background)
		self.rungame()

	def intro(self):
		ui = self.ui
		conf = self.conf

		pygame.display.set_caption("GGZBoard: game selection")

		img = ui.font.render("GGZBoard: game selection", 1, (255, 200, 0))
		ui.surface.blit(img, (20, 30))

		pygame.event.clear()

		updatescreen = 1

		x = -1
		y = -1
		selected = -1
		gamename = None

		background = pygame.Surface((ui.surface.get_width(), ui.surface.get_height()))
		background.blit(ui.surface, (0, 0))

		while not gamename:
			pygame.event.pump()
			event = pygame.event.poll()

			if event.type == KEYDOWN:
				key = event.key
				if key == K_ESCAPE or pygame.event.peek(QUIT):
					break

			if event.type == MOUSEMOTION:
				(posx, posy) = event.pos

			if event.type == MOUSEBUTTONDOWN:
				x = posx
				y = posy
				if x > 20 and x < 300 + 20:
					if ((y - 100) % 50) < 40:
						selected = (y - 100) / 50
						updatescreen = 1
						gamename = self.modulelist[selected]

			if updatescreen:
				ui.surface.blit(background, (0, 0))

				i = 0
				for module in self.modulelist:
					color = conf.tilecolor
					if i == selected:
						(r, g, b) = color
						color = (min(r + 50, 255), min(g + 50, 255), min(b + 50, 255))
					self.rect(ui.surface, (255, 255, 255), 20, i * 50 + 100, 300, 40, color)

					img = ui.smallfont.render(module, 1, (255, 200, 0))
					ui.surface.blit(img, (30, i * 50 + 100 + 10))
					i += 1

				ui.screen.blit(ui.surface, (0, 0))
				updatescreen = 0

			pygame.display.flip()

		self.loadgame(gamename)

	def rungame(self):
		conf = self.conf
		game = self.game
		ui = self.ui

		pygame.display.set_caption("GGZBoard: " + self.game.name())

		img = ui.font.render("GGZBoard: " + game.name(), 1, (255, 200, 0))
		ui.surface.blit(img, (20, 30))

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

		game.background = pygame.Surface((ui.surface.get_width(), ui.surface.get_height()))
		game.background.blit(ui.surface, (0, 0))

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
				ui.surface.blit(game.background, (0, 0))

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
						self.rect(ui.surface, (255, 255, 255), sx, sy, conf.cellwidth, conf.cellheight, color)
						if board[j][i]:
							img = self.svgsurface(game.figure(board[j][i]), conf.cellwidth, conf.cellheight)
							ui.surface.blit(img, (sx, sy))

				ui.screen.blit(ui.surface, (0, 0))
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
	ggzmode = 0
	gamename = None

	for i in range(len(sys.argv)):
		#print sys.argv[i]
		if sys.argv[i] == "--ggz":
			ggzmode = 1
		elif sys.argv[i] == "--game":
			gamename = sys.argv[i + 1]
		else:
			pass

	core = GGZBoard()
	core.load(gamename)
	core.main(ggzmode)

