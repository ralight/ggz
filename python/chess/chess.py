# Chess client
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

def rect(surface, color, x1, y1, w, h, bgcolor):
	surface.fill(bgcolor, ((x1, y1), (w, h)))
	surface.fill(color, ((x1, y1), (1, h)))
	surface.fill(color, ((x1, y1), (w, 1)))
	surface.fill(color, ((x1, y1 + h), (w, 1)))
	surface.fill(color, ((x1 + w, y1), (1, h)))

def figurename(figure):
	if figure == 0:
		return "empty"
	elif figure == 1:
		return "pawn"
	elif figure == 2:
		return "bishop"
	elif figure == 3:
		return "knight"
	elif figure == 4:
		return "rook"
	elif figure == 5:
		return "queen"
	elif figure == 6:
		return "king"
	return ""

def main():
	pygame.init()
	pygame.display.set_caption("Chess")
	screen = pygame.display.set_mode((600, 600), DOUBLEBUF)
	#pygame.display.toggle_fullscreen()

	surface = pygame.Surface((600, 600))

	pygame.event.clear()

	updatescreen = 1

	board = [0, 0, 0, 0, 0, 0, 0, 0]
	for i in range(0, 8):
		board[i] = [0, 0, 0, 0, 0, 0, 0, 0]

	board[0][0] = ("rook", "w")
	board[0][1] = ("knight", "w")
	board[0][2] = ("bishop", "w")
	board[0][3] = ("queen", "w")
	board[0][4] = ("king", "w")
	board[0][5] = ("bishop", "w")
	board[0][6] = ("knight", "w")
	board[0][7] = ("rook", "w")
	board[1][0] = ("pawn", "w")
	board[1][1] = ("pawn", "w")
	board[1][2] = ("pawn", "w")
	board[1][3] = ("pawn", "w")
	board[1][4] = ("pawn", "w")
	board[1][5] = ("pawn", "w")
	board[1][6] = ("pawn", "w")
	board[1][7] = ("pawn", "w")

	board[7][0] = ("rook", "b")
	board[7][1] = ("knight", "b")
	board[7][2] = ("bishop", "b")
	board[7][3] = ("queen", "b")
	board[7][4] = ("king", "b")
	board[7][5] = ("bishop", "b")
	board[7][6] = ("knight", "b")
	board[7][7] = ("rook", "b")
	board[6][0] = ("pawn", "b")
	board[6][1] = ("pawn", "b")
	board[6][2] = ("pawn", "b")
	board[6][3] = ("pawn", "b")
	board[6][4] = ("pawn", "b")
	board[6][5] = ("pawn", "b")
	board[6][6] = ("pawn", "b")
	board[6][7] = ("pawn", "b")

	oldx = -1
	oldy = -1
	aiturn = 0
	shift = 0
	inputallowed = 1

	ggzchess.init(ggzchess.WHITE, 2)

	while 1:
		pygame.event.pump()
		event = pygame.event.poll()

		if event.type == KEYDOWN:
			key = event.key
			if key == K_ESCAPE or pygame.event.peek(QUIT):
				break
			if key == K_r and inputallowed:
				ret = ggzchess.rochade(ggzchess.WHITE, shift)
				print "rochade", shift, ret
				kold = 4
				knew = 6
				rold = 7
				rnew = 5
				if shift:
					rold = 0
					rnew = 3
					knew = 2
				if ret:
					ggzchess.move(kold, knew, 1)
					ggzchess.move(rold, rnew, 1)
					board[0][knew] = board[0][kold]
					board[0][kold] = None
					board[0][rnew] = board[0][rold]
					board[0][rold] = None
					updatescreen = 1
					aiturn = 1
			if key == K_LSHIFT or key == K_RSHIFT:
				shift = 1

		if event.type == KEYUP:
			key = event.key
			if key == K_LSHIFT or key == K_RSHIFT:
				shift = 0

		if event.type == MOUSEMOTION:
			(posx, posy) = event.pos

		if event.type == MOUSEBUTTONDOWN and inputallowed:
			if posx % 50 < 40 and posy % 50 < 40:
				x = (posx / 50)
				y = (posy / 50)
				if oldx == -1:
					oldx = x
					oldy = y
					updatescreen = 1
				else:
					if board[oldy][oldx]:
						(figure, color) = board[oldy][oldx]
						if color is not "w":
							ret = 0
						else:
							print "MOVE", oldy * 8 + oldx, y * 8 + x
							ret = ggzchess.move(oldy * 8 + oldx, y * 8 + x, 0)
					else:
						ret = 0
					print "RET(move)", ret
					if ret:
						board[y][x] = board[oldy][oldx]
						board[oldy][oldx] = None
						aiturn = 1

						(ret, (figure)) = ggzchess.exchange(y * 8 + x)
						print "RET(exchange)", ret, figure
						if ret:
							board[y][x] = (figurename(figure), color)
					updatescreen = 1

					oldx = -1
					oldy = -1

		if updatescreen:
			for j in range(0, 8):
				for i in range(0, 8):
					color = (100, 100, 100)
					if (i + j) % 2:
						color = (150, 150, 150)
					if j == oldy and i == oldx:
						color = (200, 200, 200)
					rect(surface, (255, 255, 255), i * 50, j * 50, 40, 40, color)
					if board[j][i]:
						(gfx, color) = board[j][i]
						img = pygame.image.load("img/" + gfx + "_" + color + ".xpm")
						img = pygame.transform.scale(img, (40, 40))
						surface.blit(img, (i * 50, j * 50))

			screen.blit(surface, (0, 0))
			updatescreen = 0

		pygame.display.flip()

		if inputallowed and ggzchess.checkmate():
			print "CHECK MATE!"
			aiturn = 0
			inputallowed = 0

		if aiturn:
			aiturn = 0
			(ret, (frompos, topos)) = ggzchess.find(ggzchess.BLACK)
			print "RET(find)", ret
			if ret:
				ggzchess.move(frompos, topos, 0)
				oldy = frompos / 8
				oldx = frompos % 8
				y = topos / 8
				x = topos % 8

				print "POS", frompos, topos, "FROM", oldx, oldy, "TO", x, y
				(figure, color) = board[oldy][oldx]
				print " =>", figure, color
				if board[y][x]:
					(figure, color) = board[y][x]
					print " eats", figure, color

				board[y][x] = board[oldy][oldx]
				board[oldy][oldx] = None

				(ret, (figure)) = ggzchess.exchange(y * 8 + x)
				print "RET(exchange)", ret, figure
				if ret:
					board[y][x] = (figurename(figure), color)

			updatescreen = -1

			oldx = -1
			oldy = -1

if __name__ == "__main__":
	main()

