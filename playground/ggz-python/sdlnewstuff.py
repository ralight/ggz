# Python library for Get Hot New Stuff access
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import pygame
from pygame.locals import *
import sys
import time
import random
import os, pwd
import urllib
import xml.dom.minidom

class Stuff:
	def __init__(self):
		self.name = None
		self.type = None
		self.author = None
		self.licence = None
		self.summary = None
		self.version = None
		self.release = None
		self.releasedate = None
		self.preview = None
		self.payload = None
		self.rating = None
		self.downloads = None

		self.previewimage = None
		self.provider = None
		self.status = None
		pass

def element(stuff, key):
	value = stuff.getElementsByTagName(key)
	if len(value) >= 1:
		child = value[0].firstChild
		if child:
			return child.nodeValue
	return ""

def downloaddata():
	stufflist = []

	providers = "http://localhost:10000/hotstuff/directory/providers.xml"
	(filename, data) = urllib.urlretrieve(providers)
	print "FILENAME", filename

	dom = xml.dom.minidom.parse(filename)

	root = dom.documentElement
	print "root", root.tagName

	providers = root.getElementsByTagName("provider")
	for provider in providers:
		upload = provider.getAttribute("uploadurl")
		download = provider.getAttribute("downloadurl")
		icon = provider.getAttribute("icon")
		title = element(provider, "title")
		print "provider", upload, download, icon

		(filename, data) = urllib.urlretrieve(download)
		print "FILENAME", filename

		dom = xml.dom.minidom.parse(filename)

		root = dom.documentElement
		print "root", root.tagName

		stuffs = root.getElementsByTagName("stuff")
		for stuff in stuffs:
			s = Stuff()
			s.name = element(stuff, "name")
			s.type = element(stuff, "type")
			s.author = element(stuff, "author")
			s.licence = element(stuff, "licence")
			s.summary = element(stuff, "summary")
			s.version = element(stuff, "version")
			s.release = element(stuff, "release")
			s.releasedate = element(stuff, "releasedate")
			s.preview = element(stuff, "preview")
			s.payload = element(stuff, "payload")
			s.rating = element(stuff, "rating")
			s.downloads = element(stuff, "downloads")
			print "stuff", s.name, s.type

			try:
				(filename, data) = urllib.urlretrieve(s.preview)
				s.previewimage = pygame.image.load(filename)
			except:
				pass

			s.provider = title

			s.summary = s.summary.replace("\n", "")

			stufflist.append(s)

	return stufflist

def rect(surface, color, x1, y1, w, h, bgcolor):
	surface.fill(bgcolor, ((x1, y1), (w, h)))
	surface.fill(color, ((x1, y1), (1, h)))
	surface.fill(color, ((x1, y1), (w, 1)))
	surface.fill(color, ((x1, y1 + h), (w, 1)))
	surface.fill(color, ((x1 + w, y1), (1, h)))


def menurender(surface, stufflist, highlighted):
	titlefont = pygame.font.SysFont("Vera Sans", 24)
	font = pygame.font.SysFont("Vera Sans", 12)

	surface.fill((50, 100, 150))

	i = 10
	c = 0
	oldprovider = ""
	for s in stufflist:
		if s.provider is not oldprovider:
			rect(surface, (255, 255, 255), 10, i, 780, 30, (255, 200, 0))
			f = titlefont.render(s.provider, 1, (0, 0, 0))
			surface.blit(f, (15, i - 4))
			i += 40
			oldprovider = s.provider
		bgcolor = (100, 100, 255)
		if highlighted == c:
			bgcolor = (130, 130, 255)
		rect(surface, (255, 255, 255), 10, i, 780, 70, bgcolor)
		if s.previewimage:
			surface.blit(s.previewimage, (15, i + 3))
		f = titlefont.render(s.name, 1, (255, 255, 255))
		surface.blit(f, (90, i))
		f = font.render(s.author, 1, (255, 255, 255))
		surface.blit(f, (90, i + 30))
		f = font.render(s.version + " (" + s.releasedate + ")", 1, (255, 255, 255))
		surface.blit(f, (90, i + 45))

		f = font.render(s.summary, 1, (255, 255, 255))
		surface.blit(f, (780 - f.get_width(), i + 15))
		if not s.status:
			s.status = "not installed"
		if s.status == "installed":
			f = font.render(s.status, 1, (255, 255, 255))
			surface.blit(f, (780 - f.get_width(), i + 45))
		else:
			f = font.render(s.payload, 1, (255, 255, 255))
			surface.blit(f, (780 - f.get_width(), i + 30))
			f = font.render(s.status, 1, (120, 255, 120))
			surface.blit(f, (780 - f.get_width(), i + 45))

		i += 80
		c += 1

def homedir():
	return pwd.getpwuid(os.geteuid())[5]

def gethotnewstuff(gamename):
	""" Prepare GHNS """

	screen = pygame.display.get_surface()

	screencopy = pygame.Surface((screen.get_width(), screen.get_height()))
	screencopy.blit(screen, (0, 0))

	dark = pygame.Surface((screen.get_width(), screen.get_height()))
	dark.fill((0, 0, 0))
	dark.set_alpha(200)
	screen.blit(dark, (0, 0))

	stufflist = downloaddata()

	try:
		f = file(homedir() + "/.ggz/sdlnewstuff.install", "r")
		lines = f.readlines()
		f.close()

		for s in stufflist:
			if s.name + "::" + s.version + "\n" in lines:
				s.status = "installed"
	except:
		pass

	surface = pygame.Surface((800, 600))

	pygame.event.clear()

	selection = 0
	updatescreen = 1
	install = -1

	menurender(surface, stufflist, selection)

	while 1:
		""" Control """

		pygame.event.pump()
		event = pygame.event.poll()
		if event.type == KEYDOWN:
			key = event.key
			if key == K_ESCAPE or pygame.event.peek(QUIT):
				break
			if key == K_UP:
				if selection > 0:
					selection -= 1
					updatescreen = 1
			if key == K_DOWN:
				if selection < len(stufflist) - 1:
					selection += 1
					updatescreen = 1
			if key == K_RETURN:
				install = selection

		""" Logics """

		if install >= 0:
			s = stufflist[install]
			if s.status is not "installed":
				(filename, data) = urllib.urlretrieve(s.payload)
				target = homedir() + "/.ggz/games/" + gamename
				os.system("/bin/mkdir -p " + target)
				os.system("/bin/tar -C " + target + " -xvzf "+ filename)

				f = file(homedir() + "/.ggz/sdlnewstuff.install", "a")
				f.write(s.name + "::" + s.version + "\n")
				f.close()

				s.status = "installed"
				install = -1
				updatescreen = 1

		""" Drawing """

		if updatescreen:
			menurender(surface, stufflist, selection)
			xpos = (screen.get_width() - 800) / 2
			ypos = (screen.get_height() - 600) / 2
			screen.blit(surface, (xpos, ypos))
			updatescreen = 0
		pygame.display.flip()

	screen.blit(screencopy, (0, 0))
	pygame.display.flip()

