## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6257 2004-11-04 22:53:36Z jdorje $

SUBDIRS = .

EXTRA_DIST = Doxyfile

lib_LTLIBRARIES = libggzcore.la

# ggz_client isn't directly tied to ggzcore; it's used by ggzcore and
# ggzmod.  But since they're both in the same library, I've just placed
# it here for now - it can be moved later.
AM_CPPFLAGS = -I$(top_srcdir)/ggzmod $(LIBGGZ_INCLUDES)

include_HEADERS = ggzcore.h

libggzcore_la_LIBADD = @LDADD@ $(top_builddir)/ggzmod/libggzmod.la \
		       $(LIBGGZ_LDFLAGS) $(LIB_GGZ) $(LIBEXPAT) $(LIBPOPT)

libggzcore_la_LDFLAGS = -version-info 5:0:0

libggzcore_la_SOURCES =	conf.c \
			conf.h \
			init.c \
			game.c \
			game.h \
			gametype.c \
			gametype.h \
			ggzcore.h \
			hook.c \
			hook.h \
			module.c \
			module.h \
			net.h \
			netxml.c \
			player.c \
			player.h \
			protocol.h \
			room.c \
			room.h \
			server.c \
			server.h \
			state.c \
			state.h \
			table.c \
			table.h
