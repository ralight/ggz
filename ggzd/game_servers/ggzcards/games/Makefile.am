## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6258 2004-11-04 23:00:59Z jdorje $

SUBDIRS = .

EXTRA_DIST =

noinst_LIBRARIES = libgames.a
ggzdexecmod_PROGRAMS =
ggzdgame_DATA =
ggzdroom_DATA =

# NOTE: ggzdmod must come last so we don't include "protocol.h" from
# that directory!!  Ugh!
AM_CPPFLAGS = -I$(top_srcdir)/game_servers/ggzcards \
	-I$(top_srcdir)/game_servers/ggzcards/common \
	-I$(top_srcdir)/ggzdmod \
	$(LIBGGZ_INCLUDES)

libgames_a_DEPENDENCIES = $(top_builddir)/game_servers/ggzcards/common/libcommon.a

libgames_a_SOURCES =	bridge.c bridge.h	\
			euchre.c euchre.h	\
			fortytwo.c fortytwo.h	\
			hearts.c hearts.h	\
			lapocha.c lapocha.h	\
			spades.c spades.h	\
			suaro.c suaro.h		\
			sueca.c sueca.h		\
			whist.c whist.h

test: # nothing to be done
