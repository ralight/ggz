## Process this file with automake to produce Makefile.in

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/easysock 

ggzdexecmod_PROGRAMS = ggzd.combat
ggzdgame_DATA = combat.dsc
ggzdroom_DATA = combat.room


ggzd_combat_SOURCES = ggz.c \
		         ggz.h \
		         main.c \
						 game.c \
						 game.h \
						 combat.h \
						 combat.c \
		         protocols.h


ggzd_combat_LDADD = $(LDADD) $(top_builddir)/easysock/libeasysock.la 
ggzd_combat_LDFLAGS = -static

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

