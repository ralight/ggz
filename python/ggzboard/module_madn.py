#!/usr/bin/env python
# -*- coding: utf-8 -*-
# GGZBoard M-Ä-D-N: Mensch-ärgere-dich-nicht game module for the GGZBoard container
# Copyright (C) 2005 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random

class Game:
	def __init__(self):
		self.width = 11
		self.height = 11
		self.swaptiles = 0
		self.autoscaletiles = 1
		self.setonly = 0
		self.intersections = 0
		self.noemptytiles = 1

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.board[0][0] = ("poeppel", "y")
		self.board[0][1] = ("poeppel", "y")
		self.board[1][0] = ("poeppel", "y")
		self.board[1][1] = ("poeppel", "y")

		self.board[9][9] = ("poeppel", "r")
		self.board[9][10] = ("poeppel", "r")
		self.board[10][9] = ("poeppel", "r")
		self.board[10][10] = ("poeppel", "r")

		self.board[0][9] = ("poeppel", "b")
		self.board[0][10] = ("poeppel", "b")
		self.board[1][9] = ("poeppel", "b")
		self.board[1][10] = ("poeppel", "b")

		self.board[9][0] = ("poeppel", "g")
		self.board[9][1] = ("poeppel", "g")
		self.board[10][0] = ("poeppel", "g")
		self.board[10][1] = ("poeppel", "g")

		self.boardstyle = (None)
		self.boardstyle = resize(self.boardstyle, (self.width, self.height))

		self.boardstyle[0][0] = (150, 150, 0)
		self.boardstyle[0][1] = (150, 150, 0)
		self.boardstyle[1][0] = (150, 150, 0)
		self.boardstyle[1][1] = (150, 150, 0)

		self.boardstyle[9][9] = (150, 0, 0)
		self.boardstyle[9][10] = (150, 0, 0)
		self.boardstyle[10][9] = (150, 0, 0)
		self.boardstyle[10][10] = (150, 0, 0)

		self.boardstyle[0][9] = (0, 0, 200)
		self.boardstyle[0][10] = (0, 0, 200)
		self.boardstyle[1][9] = (0, 0, 200)
		self.boardstyle[1][10] = (0, 0, 200)

		self.boardstyle[9][0] = (0, 150, 0)
		self.boardstyle[9][1] = (0, 150, 0)
		self.boardstyle[10][0] = (0, 150, 0)
		self.boardstyle[10][1] = (0, 150, 0)

		self.boardstyle[0][6] = (0, 0, 200)
		self.boardstyle[1][5] = (0, 0, 200)
		self.boardstyle[2][5] = (0, 0, 200)
		self.boardstyle[3][5] = (0, 0, 200)
		self.boardstyle[4][5] = (0, 0, 200)

		self.boardstyle[10][4] = (0, 150, 0)
		self.boardstyle[9][5] = (0, 150, 0)
		self.boardstyle[8][5] = (0, 150, 0)
		self.boardstyle[7][5] = (0, 150, 0)
		self.boardstyle[6][5] = (0, 150, 0)

		self.boardstyle[4][0] = (150, 150, 0)
		self.boardstyle[5][1] = (150, 150, 0)
		self.boardstyle[5][2] = (150, 150, 0)
		self.boardstyle[5][3] = (150, 150, 0)
		self.boardstyle[5][4] = (150, 150, 0)

		self.boardstyle[6][10] = (150, 0, 0)
		self.boardstyle[5][9] = (150, 0, 0)
		self.boardstyle[5][8] = (150, 0, 0)
		self.boardstyle[5][7] = (150, 0, 0)
		self.boardstyle[5][6] = (150, 0, 0)

		self.boardstyle[4][1] = (110, 110, 110)
		self.boardstyle[4][2] = (110, 110, 110)
		self.boardstyle[4][3] = (110, 110, 110)
		self.boardstyle[4][4] = (110, 110, 110)
		self.boardstyle[4][6] = (110, 110, 110)
		self.boardstyle[4][7] = (110, 110, 110)
		self.boardstyle[4][8] = (110, 110, 110)
		self.boardstyle[4][9] = (110, 110, 110)
		self.boardstyle[4][10] = (110, 110, 110)

		self.boardstyle[6][0] = (110, 110, 110)
		self.boardstyle[6][1] = (110, 110, 110)
		self.boardstyle[6][2] = (110, 110, 110)
		self.boardstyle[6][3] = (110, 110, 110)
		self.boardstyle[6][4] = (110, 110, 110)
		self.boardstyle[6][6] = (110, 110, 110)
		self.boardstyle[6][7] = (110, 110, 110)
		self.boardstyle[6][8] = (110, 110, 110)
		self.boardstyle[6][9] = (110, 110, 110)

		self.boardstyle[0][4] = (110, 110, 110)
		self.boardstyle[1][4] = (110, 110, 110)
		self.boardstyle[2][4] = (110, 110, 110)
		self.boardstyle[3][4] = (110, 110, 110)
		self.boardstyle[4][4] = (110, 110, 110)
		self.boardstyle[6][4] = (110, 110, 110)
		self.boardstyle[7][4] = (110, 110, 110)
		self.boardstyle[8][4] = (110, 110, 110)
		self.boardstyle[9][4] = (110, 110, 110)

		self.boardstyle[1][6] = (110, 110, 110)
		self.boardstyle[2][6] = (110, 110, 110)
		self.boardstyle[3][6] = (110, 110, 110)
		self.boardstyle[4][6] = (110, 110, 110)
		self.boardstyle[6][6] = (110, 110, 110)
		self.boardstyle[7][6] = (110, 110, 110)
		self.boardstyle[8][6] = (110, 110, 110)
		self.boardstyle[9][6] = (110, 110, 110)
		self.boardstyle[10][6] = (110, 110, 110)

		self.boardstyle[0][5] = (110, 110, 110)
		self.boardstyle[10][5] = (110, 110, 110)
		self.boardstyle[5][0] = (110, 110, 110)
		self.boardstyle[5][10] = (110, 110, 110)

		self.isover = 0
		self.lastmove = ""

	def name(self):
		return unicode("M-Ä-D-N", "utf-8")

	def figure(self, piece):
		(gfx, color) = piece
		if color == "b":
			colorstr = "blue"
		elif color == "r":
			colorstr = "red"
		elif color == "g":
			colorstr = "green"
		elif color == "y":
			colorstr = "yellow"
		else:
			colorstr = "black"
		piecestr = "poeppel"
		return "madn/" + piecestr + "-" + colorstr + ".svg"

	def validatemove(self, fromcolor, frompos, topos):
		return 0

	def trymove(self, frompos, topos):
		return 0

	def aimove(self):
		pass

	def domove(self, frompos, topos):
		pass

	def over(self):
		return self.isover

	def toggleplayer(self):
		pass

ggzboardgame = Game()
