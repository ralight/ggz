## Process this file with automake to produce Makefile.in

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/easysock 

ggzdmod_PROGRAMS = spades
ggzdgame_DATA = spades.dsc
ggzdroom_DATA = spades.room

spades_SOURCES = card.c \
		 card.h \
                 engine_main.c \
		 engine_func.c \
		 engine.h \
		 err_func.c \
		 err_func.h \
		 ai.c \
		 options.h \
		 protocols.h \
		 socketfunc.c \
		 socketfunc.h

spades_LDADD = $(LDADD) $(top_builddir)/easysock/libeasysock.la 
spades_LDFLAGS = -static

test:
	-cp -f $(ggzdmod_PROGRAMS) $(top_builddir)/test
	-cp -f $(ggzdgame_DATA) $(top_builddir)/test/games
	-cp -f $(ggzdroom_DATA) $(top_builddir)/test/rooms

