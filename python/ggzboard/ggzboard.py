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
import ggzchess

import rsvgsdl

class Conf:
	def __init__(self):
		self.alpha = 120
		self.background = "sunset.jpg"
		self.cellwidth = 50
		self.cellheight = 50
		self.cellspace = 10
		self.marginwidth = 20
		self.marginheight = 100
		self.tilecolor = (100, 100, 100)
		self.darktilecolor = (50, 50, 50)

class Game:
	def __init__(self):
		self.width = 8
		self.height = 8

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.board[0][1] = ("piece", "w")
		self.board[0][3] = ("piece", "w")
		self.board[0][5] = ("piece", "w")
		self.board[0][7] = ("piece", "w")
		self.board[1][0] = ("piece", "w")
		self.board[1][2] = ("piece", "w")
		self.board[1][4] = ("piece", "w")
		self.board[1][6] = ("piece", "w")
		self.board[2][1] = ("piece", "w")
		self.board[2][3] = ("piece", "w")
		self.board[2][5] = ("piece", "w")
		self.board[2][7] = ("piece", "w")

		self.board[5][0] = ("piece", "b")
		self.board[5][2] = ("piece", "b")
		self.board[5][4] = ("piece", "b")
		self.board[5][6] = ("piece", "b")
		self.board[6][1] = ("piece", "b")
		self.board[6][3] = ("piece", "b")
		self.board[6][5] = ("piece", "b")
		self.board[6][7] = ("piece", "b")
		self.board[7][0] = ("piece", "b")
		self.board[7][2] = ("piece", "b")
		self.board[7][4] = ("piece", "b")
		self.board[7][6] = ("piece", "b")

	def figure(self, piece):
		(gfx, color) = piece
		if color == "w":
			colorstr = "white"
		else:
			colorstr = "black"
		if gfx == "piece":
			piecestr = "piece"
		else:
			piecestr = "king"
		return "checkers/" + piecestr + "-" + colorstr + ".svg"

	def validatemove(self, fromcolor, frompos, topos):
		(oldx, oldy) = frompos
		(x, y) = topos

		valid = 1
		reason = ""

		if oldx < 0 or oldx >= self.width or oldy < 0 or oldy >= self.height:
			valid = 0
			reason = "out of bounds"
		elif x < 0 or x >= self.width or y < 0 or y >= self.height:
			valid = 0
			reason = "out of bounds"
		elif self.board[oldy][oldx] is None:
			valid = 0
			reason = "no piece selected"
		elif self.board[y][x] is not None:
			valid = 0
			reason = "cannot move to occupied square"
		else:
			f = self.board[oldy][oldx]
			(gfx, color) = f
			if fromcolor == "":
				fromcolor = color
			if color != fromcolor:
				valid = 0
				reason = "not your piece"
			elif not (x + y) % 2:
				valid = 0
				reason = "moves only allowed on dark squares"
			else:
				if gfx == "piece":
					moves = (0, 1)
					jumps = (0, 2)
				elif gfx == "king":
					moves = (-7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7)
					jumps = (-2, 0, 2)
				if fromcolor == "w":
					factor = 1
				else:
					factor = -1
				move = (y - oldy) * factor
				if abs(y - oldy) != abs(x - oldx):
					valid = 0
					reason = "move only allowed diagonally"
				elif move not in moves:
					if move not in jumps:
						valid = 0
						reason = "move too long"
					else:
						f2 = self.board[(oldy + y) / 2][(oldx + x) / 2]
						if f2:
							(gfx2, color2) = f2
							if color2 == fromcolor:
								valid = 0
								reason = "must jump over opponent (black) piece"
						else:
							valid = 0
							reason = "must jump over a piece"
				else:
					xfactor = 1
					yfactor = 1
					if oldy > y:
						yfactor = -1
					if oldx > x:
						xfactor = -1
					for p in range(abs(y - oldy)):
						if p == 0 or p == 1:
							continue
						tmpx = oldx + p * xfactor
						tmpy = oldy + p * yfactor
						print "move-empty-check", p, tmpx, tmpy
						f = self.board[tmpy][tmpx]
						if f:
							valid = 0
							reason = "far jump is not allowed"

		if valid == 0:
			print reason
		return valid

	def trymove(self, frompos, topos):
		ret = 0

		(oldx, oldy) = frompos
		(x, y) = topos
	#	if self.board[oldy][oldx] is not None and self.board[y][x] is None:
		valid = self.validatemove("w", frompos, topos)

		if valid:
			(figure, color) = self.board[oldy][oldx]
			print "TRYMOVE", frompos, topos
			ret = 1
			#else:
			#	print reason
		return ret

	def aimove(self):
		ret = 0
		frompos = -1
		topos = -1
		value = 0
		for j in range(8):
			for i in range(8):
				f = self.board[j][i]
				if f:
					(gfx, color) = f
					if color == "b":
						frompostmp = (i, j)
						if value < 2:
							jumpmoves = ((i - 2, j - 2), (i + 2, j - 2))
							if gfx == "king":
								jumpmoves = ((i - 2, j - 2), (i + 2, j - 2), (i - 2, j + 2), (i + 2, j + 2))
							for topostmp in jumpmoves:
								valid = self.validatemove(color, frompostmp, topostmp)
								if valid:
									frompos = frompostmp
									topos = topostmp
									print "AIMOVE(2)", frompos, topos
									ret = 1
									value = 2
						if value < 1:
							movemoves = ((i - 1, j - 1), (i + 1, j - 1))
							for topostmp in movemoves:
								valid = self.validatemove(color, frompostmp, topostmp)
								if valid:
									frompos = frompostmp
									topos = topostmp
									print "AIMOVE(1)", frompos, topos
									ret = 1
									value = 1
		return (ret, frompos, topos) 

	def domove(self, frompos, topos):
		if self.validatemove("", frompos, topos):
			(oldx, oldy) = frompos
			(x, y) = topos
			self.board[y][x] = self.board[oldy][oldx]
			self.board[oldy][oldx] = None

			if y == 0 or y == 7:
				f = self.board[y][x]
				(gfx, color) = f
				gfx = "king"
				self.board[y][x] = (gfx, color)
				print "move-executor: kingification"
			if abs(y - oldy) == 2:
				if self.board[(y + oldy) / 2][(x + oldx) / 2]:
					self.board[(y + oldy) / 2][(x + oldx) / 2] = None
					print "move-executor: remove-jumped-piece"

def rect(surface, color, x1, y1, w, h, bgcolor):
	global conf

	tmp = pygame.Surface((w, h))
	tmp.fill(bgcolor)
	tmp.set_alpha(conf.alpha)
	surface.blit(tmp, (x1, y1))
	#surface.fill(bgcolor, ((x1, y1), (w, h)))
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

	pygame.init()
	pygame.display.set_caption("GGZBoard: Checkers/Draughts")
	screen = pygame.display.set_mode((800, 600), DOUBLEBUF)
	#pygame.display.toggle_fullscreen()

	#surface = pygame.Surface((800, 600))
	surface = pygame.image.load(conf.background)

	font = pygame.font.SysFont("vera", 48)
	img = font.render("GGZBoard: Checkers/Draughts", 1, (255, 200, 0))
	surface.blit(img, (20, 30))

	pygame.event.clear()

	updatescreen = 1

	game = Game()
	board = game.board

	oldx = -1
	oldy = -1
	aiturn = 0
	shift = 0
	inputallowed = 1

	width = (conf.cellwidth + conf.cellspace)
	height = (conf.cellheight + conf.cellspace)

	#ggzchess.init(ggzchess.WHITE, 2)

#	for j in range(0, game.height):
#		for i in range(0, game.width):
#			color = (100, 100, 100)
#			if (i + j) % 2:
#				color = (150, 150, 0*150)
#			if j == oldy and i == oldx:
#				color = (200, 200, 0*200)
#			sx = i * width + conf.marginwidth
#			sy = j * height + conf.marginheight
#			rect(surface, (255, 255, 255), sx, sy, conf.cellwidth, conf.cellheight, color)

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
						#board[y][x] = board[oldy][oldx]
						#board[oldy][oldx] = None
						aiturn = 1

					updatescreen = 1

					oldx = -1
					oldy = -1

		if updatescreen:
			surface.blit(game.background, (0, 0))

			for j in range(0, game.height):
				for i in range(0, game.width):
					color = conf.tilecolor
					if (i + j) % 2:
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

		if inputallowed and ggzchess.checkmate():
			print "CHECK MATE!"
			aiturn = 0
			inputallowed = 0

		if aiturn:
			aiturn = 0
			#(ret, (frompos, topos)) = ggzchess.find(ggzchess.BLACK)
			#ret = 0
			(ret, frompos, topos) = game.aimove()
			print "RET(find)", ret
			if ret:
				game.domove(frompos, topos)
				#ggzchess.move(frompos, topos, 0)
				#oldy = frompos / game.width
				#oldx = frompos % game.width
				#y = topos / game.width
				#x = topos % game.width
				#(oldx, oldy) = frompos
				#(x, y) = topos

				#print "POS", frompos, topos, "FROM", oldx, oldy, "TO", x, y
				#(figure, color) = board[oldy][oldx]
				#print " =>", figure, color
				#if board[y][x]:
				#	(figure, color) = board[y][x]
				#	print " eats", figure, color

				#board[y][x] = board[oldy][oldx]
				#board[oldy][oldx] = None

				#(ret, (figure)) = ggzchess.exchange(y * 8 + x)
				#print "RET(exchange)", ret, figure

			updatescreen = 1

			oldx = -1
			oldy = -1

if __name__ == "__main__":
	main()

