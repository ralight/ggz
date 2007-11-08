## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 9337 2007-11-08 19:33:13Z jdorje $

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
			strsep.c \
			support.h

ggz_txt_LDADD = $(LIB_GGZCORE) $(LIB_GGZ) $(LIB_READLINE) $(LIB_GETTEXT)
ggz_txt_LDFLAGS = $(GGZCORE_LDFLAGS) $(LIBGGZ_LDFLAGS)

AM_CPPFLAGS = $(GGZCORE_INCLUDES) $(LIBGGZ_INCLUDES) \
	-DPREFIX=\"$(prefix)\"
