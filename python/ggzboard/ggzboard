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
import sdlnewstuff
import playertable

path = sys.argv[0]
if path[0] == ".":
	DATAPATH = "./"
	MODULEPATH = "./"
else:
	path = os.path.abspath(path)
	DATAPATH = str("/").join(path.split("/")[:-2]) + "/share/ggz/ggzboard/"
	MODULEPATH = str("/").join(path.split("/")[:-2]) + "/lib/site-python/"
print ">>>>> ggzboard: starting with datapath", DATAPATH, "modulepath", MODULEPATH

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
	def __init__(self, resolution):
		self.origsurface = None
		self.surface = None
		self.backgroundarea = pygame.Surface(resolution)

		self.screen = None
		self.origres = resolution
		self.currentres = resolution
		self.isfullscreen = 0

		self.font = None
		self.smallfont = None

	def initfonts(self):
		self.font = pygame.font.SysFont("vera", 48)
		self.smallfont = pygame.font.SysFont("vera", 24)

	def togglefullscreen(self):
		self.isfullscreen = not self.isfullscreen
		if self.isfullscreen:
			modes = pygame.display.list_modes(0, FULLSCREEN)
			if modes != -1:
				self.currentres = modes[0]
				self.screen = pygame.display.set_mode(self.currentres, DOUBLEBUF)
				pygame.display.toggle_fullscreen()
			else:
				pygame.display.toggle_fullscreen()
		else:
			self.currentres = self.origres
			self.screen = pygame.display.set_mode(self.currentres, DOUBLEBUF)

		if self.origsurface.get_size() == self.currentres:
			self.surface = self.deepcopy(self.origsurface)
		else:
			self.surface = pygame.transform.scale(self.origsurface, self.currentres)
		self.backgroundarea = self.deepcopy(self.surface)

	def deepcopy(self, surface):
		s = pygame.Surface(surface.get_size())
		s.blit(surface, (0, 0))
		return s

	def setsurface(self, surface):
			if surface:
				self.origsurface = pygame.image.load(DATAPATH + surface)
				self.surface = pygame.transform.scale(self.origsurface, self.currentres)
				self.backgroundarea = self.deepcopy(self.surface)
			else:
				self.origsurface = pygame.Surface(self.currentres)
				self.surface = self.deepcopy(self.origsurface)
				self.backgroundarea = self.deepcopy(self.surface)

class GGZBoard:
	def __init__(self):
		self.conf = None
		self.game = None
		self.net = None
		self.modulelist = []
		self.modulefilelist = []
		self.ui = None
		self.ggzmode = 0
		self.ggzsuccess = 0

	def ggzinit(self, ggzmode):
		self.ggzmode = ggzmode

	def rect(self, surface, color, x1, y1, w, h, bgcolor):
		tmp = pygame.Surface((w, h))
		tmp.fill(bgcolor)
		tmp.set_alpha(self.conf.alpha)
		surface.blit(tmp, (x1, y1))
		if color:
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
		os.path.walk(MODULEPATH, self.walker, None)

		if gamename:
			self.loadgame(gamename)

	def loadgame(self, gamename):
		(fileobj, filename, desc) = imp.find_module("module_" + gamename, None)
		mod = imp.load_module("ggzboardgame", fileobj, filename, desc)
		fileobj.close()
		self.game = mod.ggzboardgame

		if self.ggzmode:
			(fileobj, filename, desc) = imp.find_module("net_" + gamename, None)
			mod = imp.load_module("ggzboardnet", fileobj, filename, desc)
			fileobj.close()
			self.net = mod.ggzboardnet
			self.ggzsuccess = 1

	def main(self):
		self.conf = Conf()
		self.ui = GGZBoardUI(self.conf.resolution)

		pygame.init()
		pygame.display.set_caption("GGZBoard")

		self.ui.initfonts()

		self.ui.screen = pygame.display.set_mode(self.ui.currentres, DOUBLEBUF)
		if self.conf.fullscreen:
			self.ui.toggle_fullscreen()

		self.ui.setsurface(self.conf.background)

		if not self.game:
			self.intro()
		if self.game:
			self.rungame()

	def intro(self):
		ui = self.ui
		conf = self.conf

		pygame.display.set_caption("GGZBoard: game selection")

		title = ui.font.render("GGZBoard: game selection", 1, (255, 200, 0))

		pygame.event.clear()

		updatescreen = 1

		x = -1
		y = -1
		selected = -1
		gamename = None

		(posx, posy) = pygame.mouse.get_pos()

		while not gamename:
			pygame.event.pump()
			if updatescreen:
				event = pygame.event.poll()
			else:
				event = pygame.event.wait()

			if event.type == KEYDOWN:
				key = event.key
				if key == K_ESCAPE or pygame.event.peek(QUIT):
					break
				if key == K_f:
					self.ui.togglefullscreen()
					updatescreen = 1

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
				ui.backgroundarea = ui.deepcopy(ui.surface)
				ui.backgroundarea.blit(title, (20, 30))

				i = 0
				for module in self.modulelist:
					color = conf.tilecolor
					if i == selected:
						(r, g, b) = color
						color = (min(r + 50, 255), min(g + 50, 255), min(b + 50, 255))
					self.rect(ui.backgroundarea, (255, 255, 255), 20, i * 50 + 100, 300, 40, color)

					img = ui.smallfont.render(module, 1, (255, 200, 0))
					ui.backgroundarea.blit(img, (30, i * 50 + 100 + 10))
					i += 1

				ui.screen.blit(ui.backgroundarea, (0, 0))
				updatescreen = 0

				pygame.display.flip()

		if gamename:
			self.loadgame(gamename)

	def rungame(self):
		conf = self.conf
		game = self.game
		ui = self.ui
		net = self.net

		game.init(DATAPATH)

		if self.ggzsuccess:
			ret = net.connect()
			if ret < 0:
				self.ggzsuccess = 0

		self.ui.backgroundarea = self.ui.deepcopy(self.ui.surface)

		asciiname = game.name().encode("ascii", "replace")
		pygame.display.set_caption("GGZBoard: " + asciiname)

		ggzstr = ""
		if self.ggzmode:
			ggzstr = " (running on GGZ)"
			if not self.ggzsuccess:
				ggzstr += " (unavailable)"
		title = ui.font.render("GGZBoard: " + game.name() + ggzstr, 1, (255, 200, 0))

		pygame.event.clear()

		updatescreen = 1
		rescalescreen = 1

		board = game.board

		oldx = -1
		oldy = -1
		aiturn = 0
		shift = 0

		thinking = 0
		inputallowed = 1
		if self.ggzmode:
				inputallowed = 0
				#game.toggleplayer() # FIXME: reversi specific? (player is black originally)

		(posx, posy) = pygame.mouse.get_pos()

		while 1:
			if self.ggzsuccess:
				net.handle_network()
				if net.error():
					ggzstr = " (running on GGZ)"
					ggzstr += " (unavailable)"
					title = ui.font.render("GGZBoard: " + game.name() + ggzstr, 1, (255, 200, 0))
					self.ggzsuccess = 0
					updatescreen = 1
				else:
					if net.allowed() != inputallowed:
						inputallowed = net.allowed()
						updatescreen = 1
					if net.thinking() != thinking:
						thinking = net.thinking()
						updatescreen = 1
					move = net.netmove()
					if move:
						(frompos, topos) = move
						game.toggleplayer()
						game.domove(frompos, topos)
						updatescreen = 1

			pygame.event.pump()
			if updatescreen or self.ggzsuccess:
				event = pygame.event.poll()
			else:
				event = pygame.event.wait()

			if rescalescreen:
				if game.autoscaletiles:
					conf.cellwidth = (ui.currentres[1] - 150) / game.width
					conf.cellheight = (ui.currentres[1] - 150) / game.height

				width = (conf.cellwidth + conf.cellspace)
				height = (conf.cellheight + conf.cellspace)

				rescalescreen = 0
				updatescreen = 1

			if event.type == KEYDOWN:
				key = event.key
				if key == K_ESCAPE or pygame.event.peek(QUIT):
					break
				if key == K_f:
					self.ui.togglefullscreen()
					rescalescreen = 1
				#if key == K_r:
				#	updatescreen = 1
				if key == K_u:
					sdlnewstuff.gethotnewstuff(game.name())
				if key == K_p:
					playertable.show()
				if key == K_s:
					pygame.image.save(ui.screen, "ggzboard-screenshot.bmp")

			if event.type == MOUSEMOTION:
				(posx, posy) = event.pos

			if event.type == MOUSEBUTTONDOWN and inputallowed and not thinking:
				considered = 0
				if game.intersections:
					x = (posx - conf.marginwidth + 5) % width
					y = (posy - conf.marginheight + 5) % height
					if x < 10 and y < 10:
						x = ((posx - conf.marginwidth + 5) / width)
						y = ((posy - conf.marginheight + 5) / height)
						considered = 1
				else:
					x = (posx - conf.marginwidth) % width
					y = (posy - conf.marginheight) % height
					if x < conf.cellwidth and y < conf.cellheight:
						x = ((posx - conf.marginwidth) / width)
						y = ((posy - conf.marginheight) / height)
						considered = 1
				if considered:
					if x >= game.width or y >= game.height or x < 0 or y < 0:
						# ignore
						pass
					elif oldx == -1:
						if game.setonly:
							ret = game.trymove(None, (x, y))
							if ret:
								if self.ggzmode:
									net.domove(None, (x, y))
								else:
									game.toggleplayer()
									game.domove(None, (x, y))
									aiturn = 1
									thinking = 1
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
							if self.ggzmode:
								net.domove(frompos, topos)
							else:
								game.toggleplayer()
								game.domove(frompos, topos)
								aiturn = 1
								thinking = 1

						updatescreen = 1

						oldx = -1
						oldy = -1

			if self.ggzsuccess:
				if net.modified:
					updatescreen = 1
					net.modified = 0

			if updatescreen:
				ui.backgroundarea = ui.deepcopy(ui.surface)
				ui.backgroundarea.blit(title, (20, 30))

				if not inputallowed:
					w = ui.backgroundarea.get_width()
					h = ui.backgroundarea.get_height()
					self.rect(ui.backgroundarea, None, 0, 0, w, h, (0, 0, 0))

				for j in range(0, game.height):
					for i in range(0, game.width):
						color = conf.tilecolor
						if game.swaptiles and (i + j) % 2:
							color = conf.darktilecolor
						if game.boardstyle:
							style = game.boardstyle[j][i]
							if style:
								(r, g, b) = color
								(rd, gd, bd) = style
								color = (min(r + rd, 255), min(g + gd, 255), min(b + bd, 255))
							elif game.noemptytiles:
								continue
						if j == oldy and i == oldx:
							(r, g, b) = color
							color = (min(r + 50, 255), min(g + 50, 255), min(b + 50, 255))
						sx = i * width + conf.marginwidth
						sy = j * height + conf.marginheight
						if not game.intersections or (j != game.height - 1 and i != game.width - 1):
							self.rect(ui.backgroundarea, (255, 255, 255), sx, sy, conf.cellwidth, conf.cellheight, color)
						if board[j][i]:
							img = self.svgsurface(game.figure(board[j][i]), conf.cellwidth, conf.cellheight)
							if game.intersections:
								ui.backgroundarea.blit(img, (sx - conf.cellwidth / 2, sy - conf.cellwidth / 2))
							else:
								ui.backgroundarea.blit(img, (sx, sy))

				if self.ggzsuccess:
					w = ui.backgroundarea.get_width()
					h = ui.backgroundarea.get_height()
					if net.playernames:
						i = 0
						for player in net.playernames:
							x = w - conf.marginwidth
							y = conf.marginheight + i * 60
							caption = ui.smallfont.render("Player " + str(i + 1) + ":", 1, (255, 200, 0))
							ui.backgroundarea.blit(caption, (x - caption.get_width(), y))
							player = ui.smallfont.render(player, 1, (255, 200, 0))
							ui.backgroundarea.blit(player, (x - player.get_width(), y + 30))
							i += 1

				if thinking:
					w = ui.backgroundarea.get_width()
					h = ui.backgroundarea.get_height()
					x = w - conf.marginwidth
					y = h - conf.marginheight - 60
					caption = ui.smallfont.render("Thinking...", 1, (255, 200, 0))
					ui.backgroundarea.blit(caption, (x - caption.get_width(), y))

				ui.screen.blit(ui.backgroundarea, (0, 0))
				updatescreen = 0

				pygame.display.flip()

			if inputallowed and game.over():
				aiturn = 0
				inputallowed = 0
				updatescreen = 1

			if aiturn:
				aiturn = 0
				thinking = 0
				(ret, frompos, topos) = game.aimove()
				print "RET(find)", ret
				if ret:
					game.toggleplayer()
					game.domove(frompos, topos)

				updatescreen = 1

				oldx = -1
				oldy = -1

if __name__ == "__main__":
	ggzmode = 0
	help = 0
	gamename = None
	param = 0

	for i in range(1, len(sys.argv)):
		#print sys.argv[i]
		if sys.argv[i] == "-g" or sys.argv[i] == "--ggz":
			ggzmode = 1
		elif sys.argv[i] == "-h" or sys.argv[i] == "--help":
			help = 1
		elif sys.argv[i] == "-G" or sys.argv[i] == "--game":
			gamename = sys.argv[i + 1]
			param = 1
		else:
			if param:
				param = 0
			else:
				print "Unknown option:", sys.argv[i]

	if help:
		print "GGZBoard - common board game client for GGZ"
		print "Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>"
		print "Published under GNU GPL conditions"
		print ""
		print "Options:"
		print "[-h | --help           ] Display this game help"
		print "[-g | --ggz            ] Request game in GGZ mode"
		print "[-G | --game <gamename>] Skip intro screen and launch game directly"
		sys.exit(0)

	core = GGZBoard()
	core.ggzinit(ggzmode)
	core.load(gamename)
	core.main()

