## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3462 2002-02-25 09:15:28Z jdorje $

SUBDIRS = .

EXTRA_DIST =

noinst_LIBRARIES = libgames.a
ggzdexecmod_PROGRAMS =
ggzdgame_DATA =
ggzdroom_DATA =

# NOTE: ggzdmod must come last so we don't include "protocol.h" from
# that directory!!  Ugh!
INCLUDES = -I.. -I../common -I$(top_srcdir)/ggzdmod

libgames_a_DEPENDENCIES = ../common/libcommon.a

libgames_a_SOURCES =	bridge.c bridge.h	\
			euchre.c euchre.h	\
			hearts.c hearts.h	\
			lapocha.c lapocha.h	\
			spades.c spades.h	\
			suaro.c suaro.h		\
			sueca.c sueca.h		\
			whist.c whist.h

test: # nothing to be done
