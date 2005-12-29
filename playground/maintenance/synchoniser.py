#!/usr/bin/env python
#
# Checks for file synchronisation consistency in GGZ SVN

import os

pos_cc = ("kde-client", "gnome-client", "gtk-client", "txt-client")
pos_games = ("gtk-games", "kde-games", "sdl-games")
pos_other = ("libggz", "ggz-client-libs", "ggzd", "docs", "grubby", "python", "utils")

positions_readmeggz = pos_cc + pos_games + pos_other
positions_quickstartggz = pos_cc + pos_games + ("libggz", "ggz-client-libs", "docs", "python")
positions_ggzm4 = pos_cc + pos_games + ("libggz", "ggz-client-libs", "ggzd", "grubby", "utils")
positions_autogensh = pos_cc + pos_games + ("libggz", "ggz-client-libs", "ggzd", "docs", "grubby", "utils")
positions_i18nmk = ("ggz-client-libs/m4", "gnome-client/m4", "grubby/m4", "gtk-client/m4", "gtk-games/m4",
	"python/po", "txt-client/m4")
positions_tttai = ("playground/ggz-ruby/ai-ttt", "ggzd/game_servers/tictactoe")
positions_chessai = ("python/chess", "grubby/games/guru-chess", "ggzd/game_servers/chess")

files = {}
files["README.GGZ"] = positions_readmeggz
files["QuickStart.GGZ"] = positions_quickstartggz
files["m4/ggz.m4"] = positions_ggzm4
files["autogen.sh"] = positions_autogensh
files["i18n.mk"] = positions_i18nmk
files["ttt-ai.c"] = positions_tttai
files["ttt-ai.h"] = positions_tttai
files["ai.h"] = positions_chessai
files["ai.c"] = positions_chessai

def filesize(f):
		size = 0
		try:
			size = os.stat(p + "/" + fn)[6]
		except:
			size = 0
		return size

for fn in files:
	positions = files[fn]
	print "++", fn
	hash = {}
	for p in positions:
		#print p
		f = p + "/" + fn
		size = filesize(f)
		if not hash.has_key(size):
			hash[size] = []
		hash[size].append(f)
	if len(hash) == 1:
		print "ok!"
	else:
		print "differences detected!"
		print len(hash), "groups:"
		for size in hash:
			fgroup = hash[size]
			print size, ":", fgroup

