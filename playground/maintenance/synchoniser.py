#!/usr/bin/env python
#
# Checks for file synchronisation consistency in GGZ SVN
# Enforces consistency per item with --copy <file> <master-position>

import os
import sys

pos_cc = ("kde-client", "gnome-client", "gtk-client", "txt-client")
pos_games = ("gtk-games", "kde-games", "sdl-games")
pos_other = ("libggz", "ggz-client-libs", "ggzd", "docs", "grubby", "python", "utils", "java")

positions_readmeggz = pos_cc + pos_games + pos_other + ("community",)
positions_quickstartggz = pos_cc + pos_games + ("libggz", "ggz-client-libs", "docs", "python")
positions_ggzm4 = pos_cc + pos_games + ("libggz", "ggz-client-libs", "ggzd", "grubby", "utils")
positions_autogensh = pos_cc + pos_games + ("libggz", "ggz-client-libs", "ggzd", "docs", "grubby", "utils")
positions_i18nmk = ("ggz-client-libs/m4", "gnome-client/m4", "grubby/m4", "gtk-client/m4", "gtk-games/m4",
	"python/po", "txt-client/m4")
positions_tttai = ("playground/ggz-ruby/ai-ttt", "ggzd/game_servers/tictactoe")
positions_chessai = ("python/chess", "grubby/games/guru-chess", "ggzd/game_servers/chess")
positions_news = pos_cc + pos_games + pos_other + ("community",)
positions_authors = pos_cc + pos_games + pos_other

files = {}
files["README.GGZ"] = positions_readmeggz
files["QuickStart.GGZ"] = positions_quickstartggz
files["NEWS"] = positions_news
files["AUTHORS"] = positions_authors
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
			size = os.stat(f)[6]
		except:
			size = 0
		return size

def filecopy(fnsrc, fndst):
	#print "-- copy!", fnsrc, fndst
	f = open(fnsrc)
	f2 = file(fndst, "w")
	contents = f.read()
	f2.write(contents)
	#while(f.read())
	f.close()
	f2.close()

def check(files):
	for (fn, positions) in files.items():
		print "++", fn
		hash = {}
		for p in positions:
			#print p
			f = os.path.join(p, fn)
			size = filesize(f)
			hash.setdefault(size, [])
			hash[size].append(f)
		if len(hash) == 1:
			print "ok!"
		else:
			print "differences detected!"
			print len(hash), "groups:"
			for (size, fgroup) in hash.items():
				print size, ":", fgroup

def copy(copyfile, masterpos, files):
	if not files.has_key(copyfile):
		print "error - no such file " + copyfile + "!"
		return
	positions = files[copyfile]
	if not masterpos in positions:
		print "error - no such master position " + masterpos + "!"
		return
	for p in positions:
		if not p == masterpos:
			print "... to", p
			fnsrc = os.path.join(masterpos, copyfile)
			fndst = os.path.join(p, copyfile)
			filecopy(fnsrc, fndst)

masterpos = None
copyfile = None

if len(sys.argv) == 4:
	if sys.argv[1] == "--copy":
		copyfile = sys.argv[2]
		masterpos = sys.argv[3]
		print "Copy", copyfile, "from", masterpos, "..."
		copy(copyfile, masterpos, files)
		sys.exit(0)

check(files)

