## Process this file with automake to produce Makefile.in

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/easysock 

ggzdexecmod_PROGRAMS = ggzd.reversi
ggzdgame_DATA = reversi.dsc
ggzdroom_DATA = reversi.room


ggzd_reversi_SOURCES = ggz.c \
		         ggz.h \
		         main.c \
						 game.c \
						 game.h \
		         protocols.h


ggzd_reversi_LDADD = $(LDADD) $(top_builddir)/easysock/libeasysock.la 
ggzd_reversi_LDFLAGS = -static

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

