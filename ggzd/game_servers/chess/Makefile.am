## Process this file with automake to produce Makefile.in

SUBDIRS = libcgc .

INCLUDES = -Ilibcgc -I$(top_srdir)/easysock

ggzdexecmod_PROGRAMS = ggzd.chess
ggzdgame_DATA = chess.dsc
ggzdroom_DATA = chess.room


ggzd_chess_SOURCES = ggz.c \
		         ggz.h \
		         main.c \
             chess.h \
             game.h \
             game.c \
		         protocols.h


ggzd_chess_LDADD = libcgc/libcgc.a -leasysock
#ggzd_chess_LDFLAGS = -static

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

