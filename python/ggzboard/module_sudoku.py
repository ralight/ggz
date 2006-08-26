# GGZBoard Sudoku: Sudoku game module for the GGZBoard container
# Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

from Numeric import *
import random
import pygame
import time

class Game:
	def __init__(self):
		self.width = 11
		self.height = 11
		self.swaptiles = 0
		self.autoscaletiles = 1
		self.setonly = 1
		self.intersections = 0
		self.noemptytiles = 1
		self.players = 1
		self.turnplayer = 0

		self.boardhints = None

		self.boardstyle = None
		self.boardstyle = resize(self.boardstyle, (self.width, self.height))
		for i in range(self.width):
			for j in range(self.height):
				if i != 3 and i != 7:
					if j != 3 and j != 7:
						self.boardstyle[j][i] = (255, 255, 255)

		self.board = (None)
		self.board = resize(self.board, (self.width, self.height))

		self.isover = 0

		self.dice = None
		self.playercolours = None
		self.winner = None
		self.help = None

		self.selection = None
		self.boardwidth = 50 # !!!
		self.boardheight = 50 # !!!

		self.verbose = 0

	# !!!
	def selectpiece(self, placepos):
		(x, y) = placepos
		if self.boardstyle[y][x] == (50, 50, 255):
			return

		self.selection = [-1, 1, 2, 3, 4, 5, 6, 7, 8, 9]

		if self.board[y][x] == 0:
			self.selection.pop(0)
		for i in range(self.width):
			if self.board[y][i] in self.selection:
				self.selection.remove(self.board[y][i])
		for j in range(self.height):
			if self.board[j][x] in self.selection:
				self.selection.remove(self.board[j][x])

		bx = (x / 4) * 4
		by = (y / 4) * 4
		for i in range(bx, bx + 3):
			for j in range(by, by + 3):
				#print "---", (i, j)
				if self.board[j][i] in self.selection:
					#print "block", self.board[j][i]
					self.selection.remove(self.board[j][i])

	def init(self, path):
		board = self.sudoku_create(9)

		for j in range(9):
			for i in range(9):
				ni = i + (i / 3)
				nj = j + (j / 3)
				self.board[nj][ni] = board[i][j]

		for i in range(self.width):
			for j in range(self.height):
				if self.board[j][i] and self.board[j][i] != 0:
					self.boardstyle[j][i] = (50, 50, 255)

	def sudoku_create(self, size):
		permutations = size * size
		obfuscations = size * size * 2

		blockmode = 1

		if blockmode:
			chains = self.sudoku_create_initial_blocks(size, size)
		else:
			chains = self.sudoku_create_initial(size, size)
		self.sudoku_permutate(chains, size, size, permutations, blockmode)
		self.sudoku_obfuscate(chains, size, size, obfuscations)

		if self.verbose:
			print "BOARD", chains
		return chains

	def sudoku_obfuscate(self, chains, width, height, count):
		if self.verbose:
			print "** obfuscation -", count, "times **"

		shadow = []
		for chain in chains:
			shadow.append(chain[:])

		for n in range(count):
			x = random.randint(0, width - 1)
			y = random.randint(0, height - 1)
			save = chains[x][y]
			if save == 0:
				continue
			if self.verbose:
				print "- try", (x, y), "having value", save
			ambiguous = 0
			for i in range(1, width + 1):
				shadow[x][y] = i
				if i == save:
					continue
				if self.sudoku_checkboard(shadow, width, height):
					if self.verbose:
						print "- valid for", i, "too!"
					ambiguous = 1
			chains[x][y] = 0
			empties = 0
			for i in range(0, width):
				empty = 1
				for j in range(0, height):
					if chains[i][j] != 0:
						empty = 0
				if empty:
					empties += 1
			if empties > 1:
				if self.verbose:
					print "- double clear column detected"
				ambiguous = 1
			empties = 0
			for j in range(0, height):
				empty = 1
				for i in range(0, width):
					if chains[i][j] != 0:
						empty = 0
				if empty:
					empties += 1
			if empties > 1:
				if self.verbose:
					print "- double clear row detected"
				ambiguous = 1
			chains[x][y] = save
			shadow[x][y] = save
			if not ambiguous:
				chains[x][y] = 0

		if self.verbose:
			print "SHADOW", shadow

	def sudoku_checkboard(self, chains, width, height):
		invalid = 0
		for j in range(height):
			nums = {}
			for i in range(width):
				if nums.has_key(chains[i][j]):
					invalid = 1
					break
				else:
					nums[chains[i][j]] = 1
			if invalid:
				break
		if not invalid:
			for i in range(width):
				nums = {}
				for j in range(height):
					if nums.has_key(chains[i][j]):
						invalid = 1
						break
					else:
						nums[chains[i][j]] = 1
				if invalid:
					break
		return not invalid

	def sudoku_permutate(self, chains, width, height, permutations, blockmode):
		if self.verbose:
			print "** permutation of board -", permutations, "times **"

		stats_vperms = 0
		stats_hperms = 0

		for i in range(permutations):
			mode = random.randint(0, 1)
			if mode == 0:
				# swap rows
				max = height
			else:
				# swap columns
				max = width
			n = random.randint(0, max - 1)
			n2 = n
			while n == n2:
				n2 = random.randint(0, max - 1)
			if blockmode:
				while (n / 3 != n2 / 3) or n == n2:
					n2 = random.randint(0, max - 1)
			if mode == 0:
				stats_vperms += 1
				for i in range(width):
					tmp = chains[i][n]
					chains[i][n] = chains[i][n2]
					tmp = chains[i][n2] = tmp
			else:
				stats_hperms += 1
				for j in range(height):
					tmp = chains[n][j]
					chains[n][j] = chains[n2][j]
					tmp = chains[n2][j] = tmp

		if self.verbose:
			print "- found final board", chains
			print "- board validity?", self.sudoku_checkboard(chains, width, height)
			print "- number of horizontal permutations", stats_hperms
			print "- number of vertical permutations", stats_vperms

	def permute(self, seq):
		seq = seq[1:] + seq[:1]
		return seq

	def sudoku_create_initial_blocks(self, width, height):
		if self.verbose:
			print "** creation of board [", width, ",", height, "]",
			print "honouring blocks **"

		chain0 = []
		chain1 = []
		chain2 = []
		for i in range(3):
			chain0.append(i + 1)
			chain1.append(i + 4)
			chain2.append(i + 7)
		block = [chain0, chain1, chain2]

		if self.verbose:
			print "block", block

		chains = []
		for i in range(3):
			l0 = block[0][:] + block[1][:] + block[2][:]
			l1 = block[2][:] + block[0][:] + block[1][:]
			l2 = block[1][:] + block[2][:] + block[0][:]

			if self.verbose:
				print "lines", l0, l1, l2

			chains.append(l0[:])
			chains.append(l1[:])
			chains.append(l2[:])

			for j in range(3):
				block[j] = self.permute(block[j])

		if self.verbose:
			print "chains", chains

		return chains

	def sudoku_create_initial(self, width, height):
		if self.verbose:
			print "** creation of board [", width, ",", height, "] **"

		chain = []
		chains = []
		for i in range(width):
			chain.append(i + 1)
		xchain = chain[:]
		for j in range(height):
			#print "xchain", xchain
			chains.append(xchain)
			xchain = xchain[1:] + xchain[:1]

		return chains

	def name(self):
		return _("Sudoku")

	def figure(self, piece):
		return None

	# !!!
	def figuregfx(self, piece):
		font = pygame.font.SysFont("vera", 48)
		font.set_bold(1)
		s = str(piece)
		if piece == -1:
			s = "---"
		img = font.render(s, 1, (0, 255, 0))
		surf = pygame.Surface((50, 50))
		x = (surf.get_width() - img.get_width()) / 2
		y = (surf.get_height() - img.get_height()) / 2
		surf.set_alpha(100)
		surf.blit(img, (x, y))
		return surf

	def validatemove(self, topos):
		(x, y) = topos

		valid = 1
		reason = ""

		if x < 0 or x >= self.width or y < 0 or y >= self.height:
			valid = 0
			reason = "out of bounds"
		elif self.boardstyle[y][x] is None:
			valid = 0
			reason = "empty field"
		else:
			#print "SELECTED", self.selected
			if self.selected:
				for i in range(self.width):
					if self.board[y][i] == self.selected:
						valid = 0
						reason = "double entry"
				for j in range(self.height):
					if self.board[j][x] == self.selected:
						valid = 0
						reason = "double entry"
				# FIXME: check for blockmode
			else:
				valid = 0
				reason = "no piece selected"

		if valid == 0:
			print reason
		return valid

	def trymove(self, frompos, topos):
		(x, y) = topos
		valid = self.validatemove(topos)

		if valid:
			print "TRYMOVE", topos
			return 1
		return 0

	def aimove(self):
		return (0, None, None)

	def domove(self, frompos, topos):
		if self.validatemove(topos):
			(x, y) = topos
			sel = self.selected
			if sel == -1:
				sel = 0
			self.board[y][x] = sel
			self.selected = None

		empty = 0
		for i in range(self.width):
			for j in range(self.height):
				if self.board[j][i] is not None:
					if self.board[j][i] == 0:
						empty = 1
		if not empty:
			self.winner = 0
			self.isover = 1

	def over(self):
		return self.isover

	def toggleplayer(self):
		pass

ggzboardgame = Game()

