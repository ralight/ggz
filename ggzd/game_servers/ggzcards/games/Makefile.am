## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2832 2001-12-09 21:41:07Z jdorje $

SUBDIRS = .

EXTRA_DIST =

noinst_LIBRARIES = libgames.a
ggzdexecmod_PROGRAMS =
ggzdgame_DATA =
ggzdroom_DATA =

INCLUDES = -I$(top_srcdir)/ggzdmod -I..
libgames_a_DEPENDENCIES =
libgames_a_SOURCES = suaro.c suaro.h \
			spades.c spades.h \
			hearts.c hearts.h \
			bridge.c bridge.h \
			lapocha.c lapocha.h \
			euchre.c euchre.h \
			sueca.c sueca.h

test: # nothing to be done
