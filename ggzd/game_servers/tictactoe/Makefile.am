## Process this file with automake to produce Makefile.in

EXTRA_DIST = tictactoe.dsc tictactoe.room

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/easysock 

ggzdmod_PROGRAMS = ggzd.tictactoe
ggzdgame_DATA = tictactoe.dsc
ggzdroom_DATA = tictactoe.room

ggzd_tictactoe_SOURCES = ggz.c \
		         ggz.h \
		         main.c \
		         game.c \
                         game.h \
		         protocols.h


ggzd_tictactoe_LDADD = $(LDADD) $(top_builddir)/easysock/libeasysock.la 
ggzd_tictactoe_LDFLAGS = -static

test:
	-cp -f $(ggzdmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

