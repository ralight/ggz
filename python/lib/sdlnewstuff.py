# Python/SDL library for Get Hot New Stuff access
# Copyright (C) 2004 - 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

import pygame
from pygame.locals import *
import sys
import time
import os, pwd
import urllib
import xml.dom.minidom
import gettext
import imp

""" Internationalization """

gettext.install("ggzpython", "/usr/local/share/locale", 1)

""" GHNS data entry class """

class Stuff:
	def __init__(self):
		self.name = None
		self.category = None
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

""" SDLNewstuff configuration class """

class Configuration:
	def __init__(self):
		self.providers = "http://ftp.ggzgamingzone.org/pub/hotstuff/providers.xml"
		self.installdir = None

		self.unpack = 1
		self.keyboard = 1
		self.mouse = 1

		self.alpha_darkness = 150
		self.color_background = (50, 100, 150)
		self.color_foreground = (255, 255, 255)
		self.color_box = (100, 100, 255)
		self.color_activebox = (130, 130, 255)
		self.color_titlebox = (255, 120, 0)
		self.color_action = (120, 255, 120)

		self.lang = os.getenv("LANG", None)
	
	def parse(self, conf):
		for attr in vars(self):
			if hasattr(conf, attr):
				setattr(self, attr, getattr(conf, attr))

def element(stuff, key):
	value = stuff.getElementsByTagName(key)
	if len(value) >= 1:
		child = value[0].firstChild
		if child:
			return child.nodeValue
	return ""

def downloaddata(conf, category):
	stufflist = []

	providers = conf.providers
	try:
		(filename, data) = urllib.urlretrieve(providers)
	except:
		return None
	print "FILENAME", filename

	try:
		dom = xml.dom.minidom.parse(filename)
	except:
		return None

	root = dom.documentElement
	print "root", root.tagName

	providers = root.getElementsByTagName("provider")
	for provider in providers:
		upload = provider.getAttribute("uploadurl")
		download = provider.getAttribute("downloadurl")
		icon = provider.getAttribute("icon")
		title = element(provider, "title")
		print "provider", upload, download, icon

		try:
			(filename, data) = urllib.urlretrieve(download)
		except:
			continue
		print "FILENAME", filename

		dom = xml.dom.minidom.parse(filename)

		root = dom.documentElement
		print "root", root.tagName

		stuffs = root.getElementsByTagName("stuff")
		for stuff in stuffs:
			s = Stuff()
			s.name = element(stuff, "name")
			s.category = stuff.getAttribute("category")
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
			print "stuff", s.name, s.category

			if s.category != category:
				continue

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

def menurender(surface, stufflist, highlighted, conf):
	titlefont = pygame.font.SysFont("Vera Sans", 24)
	font = pygame.font.SysFont("Vera Sans", 12)

	surface.fill(conf.color_background)

	if not stufflist:
		i = 10
		rect(surface, conf.color_foreground, 10, i, 780, 30, conf.color_titlebox)
		if stufflist is None:
			f = titlefont.render(_("Error: couldn't download GHNS feed"), 1, (0, 0, 0))
		else:
			f = titlefont.render(_("GHNS feed contains no entries"), 1, (0, 0, 0))
		surface.blit(f, (15, i - 4))
		return

	i = 10
	c = 0
	if highlighted > 5:
		i -= (highlighted - 5) * 80
	oldprovider = ""
	for s in stufflist:
		if s.provider is not oldprovider:
			rect(surface, conf.color_foreground, 10, i, 780, 30, conf.color_titlebox)
			f = titlefont.render(s.provider, 1, (0, 0, 0))
			surface.blit(f, (15, i - 4))
			i += 40
			oldprovider = s.provider
		bgcolor = conf.color_box
		if highlighted == c:
			bgcolor = conf.color_activebox
		rect(surface, conf.color_foreground, 10, i, 780, 70, bgcolor)
		if s.previewimage:
			surface.blit(s.previewimage, (15, i + 3))
		f = titlefont.render(s.name, 1, conf.color_foreground)
		surface.blit(f, (90, i))
		f = font.render(s.author, 1, conf.color_foreground)
		surface.blit(f, (90, i + 30))
		f = font.render(s.version + " (" + s.releasedate + ")", 1, conf.color_foreground)
		surface.blit(f, (90, i + 45))

		f = font.render(s.summary, 1, conf.color_foreground)
		surface.blit(f, (780 - f.get_width(), i + 15))
		if not s.status:
			s.status = _("not installed")
		if s.status == _("installed"):
			f = font.render(s.status, 1, conf.color_foreground)
			surface.blit(f, (780 - f.get_width(), i + 45))
		else:
			f = font.render(s.payload, 1, conf.color_foreground)
			surface.blit(f, (780 - f.get_width(), i + 30))
			f = font.render(s.status, 1, conf.color_action)
			surface.blit(f, (780 - f.get_width(), i + 45))

		i += 80
		c += 1

def homedir():
	return pwd.getpwuid(os.geteuid())[5]

class GHNSEngine:
	def __init__(self):
		self.conf = Configuration()

		os.system("mkdir -p " + homedir() + "/.sdlnewstuff")

		try:
			(fileobj, filename, desc) = imp.find_module(gamename, [homedir() + "/.sdlnewstuff/"])
			config = imp.load_module("config", fileobj, filename, desc)
			fileobj.close()
			conf.parse(config.conf)
		except:
			pass

	def gethotnewstuff(self, category):
		""" Prepare GHNS """

		screen = pygame.display.get_surface()

		screencopy = pygame.Surface((screen.get_width(), screen.get_height()))
		screencopy.blit(screen, (0, 0))

		conf = self.conf

		if conf.alpha_darkness:
			dark = pygame.Surface((screen.get_width(), screen.get_height()))
			dark.fill((0, 0, 0))
			dark.set_alpha(conf.alpha_darkness)
			screen.blit(dark, (0, 0))

		stufflist = downloaddata(conf, category)

		try:
			f = file(homedir() + "/.sdlnewstuff/sdlnewstuff.install", "r")
			lines = f.readlines()
			f.close()

			for s in stufflist:
				if s.name + "::" + s.version + "\n" in lines:
					s.status = _("installed")
		except:
			pass

		surface = pygame.Surface((800, 600))

		pygame.event.clear()

		selection = 0
		updatescreen = 1
		install = -1

		while 1:
			""" Control """

			pygame.event.pump()
			if updatescreen:
				event = pygame.event.poll()
			else:
				event = pygame.event.wait()
			if conf.keyboard and event.type == KEYDOWN:
				key = event.key
				if key == K_ESCAPE or pygame.event.peek(QUIT):
					break
				if key == K_UP:
					if stufflist and selection > 0:
						selection -= 1
						updatescreen = 1
				if key == K_DOWN:
					if stufflist and selection < len(stufflist) - 1:
						selection += 1
						updatescreen = 1
				if key == K_RETURN:
					install = selection
			if conf.mouse and event.type == MOUSEMOTION and stufflist:
				(posx, posy) = event.pos
				left = (screen.get_width() - 800) / 2
				up = (screen.get_height() - 600) / 2
				if posx > left + 10 and posx < left + 800 - 10:
					c = 0
					i = 10
					oldprovider = ""
					for s in stufflist:
						if s.provider is not oldprovider:
							i += 40
							oldprovider = s.provider
						if posy > up + i and posy < up + i + 70:
							if selection is not c:
								selection = c
								updatescreen = 1
						i += 80
						c += 1

			if conf.mouse and event.type == MOUSEBUTTONDOWN:
				install = selection

			""" Logics """

			if install >= 0 and stufflist and len(stufflist) > 0:
				s = stufflist[install]
				if s.status != _("installed"):
					try:
						(filename, data) = urllib.urlretrieve(s.payload)
						if conf.installdir:
							target = homedir() + "/" + conf.installdir
						else:
							target = homedir() + "/.ggz/games/" + gamename
						os.system("mkdir -p " + target)
						if conf.unpack:
							ret = os.system("tar -C " + target + " -xvzf " + filename)
						else:
							ret = os.system("cp " + filename + " " + target)

						if ret == 0:
							f = file(homedir() + "/.sdlnewstuff/sdlnewstuff.install", "a")
							f.write(s.name + "::" + s.version + "\n")
							f.close()

							s.status = _("installed")
						else:
							s.status = _("error")
					except:
						s.status = _("error")
					
					install = -1
					updatescreen = 1

			""" Drawing """

			if updatescreen:
				menurender(surface, stufflist, selection, conf)
				xpos = (screen.get_width() - 800) / 2
				ypos = (screen.get_height() - 600) / 2
				screen.blit(surface, (xpos, ypos))
				updatescreen = 0
			pygame.display.flip()

		screen.blit(screencopy, (0, 0))
		pygame.display.flip()

# Get a new engine

def ghnsengine():
	return GHNSEngine()

# Legacy wrapper functions for compatibility

def gethotnewstuff(category):
	engine = GHNSEngine()
	engine.gethotnewstuff(category)

