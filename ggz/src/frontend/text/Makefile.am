## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6642 2005-01-13 01:55:40Z jdorje $

SUBDIRS = .

bin_PROGRAMS = ggz-txt

ggz_txt_SOURCES = 	game.c \
			game.h \
			input.c \
			input.h \
			loop.c \
			loop.h \
			main.c \
			motd.c \
			motd.h \
			output.c \
			output.h \
			server.c \
			server.h \
			state.c \
			state.h \
			support.h

ggz_txt_LDADD = $(LIB_GGZCORE) $(LIB_GGZMOD) $(LIB_GGZ) $(LIB_READLINE) $(LIB_CURSES)
ggz_txt_LDFLAGS = $(GGZCORE_LDFLAGS) $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

AM_CPPFLAGS = $(GGZCORE_INCLUDES) $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES)

