## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3439 2002-02-22 23:33:47Z jdorje $

SUBDIRS = .

EXTRA_DIST =

noinst_LIBRARIES = libgames.a
ggzdexecmod_PROGRAMS =
ggzdgame_DATA =
ggzdroom_DATA =

INCLUDES = -I$(top_srcdir)/ggzdmod -I..
libgames_a_DEPENDENCIES =
libgames_a_SOURCES =	bridge.c bridge.h	\
			euchre.c euchre.h	\
			hearts.c hearts.h	\
			lapocha.c lapocha.h	\
			spades.c spades.h	\
			suaro.c suaro.h		\
			sueca.c sueca.h		\
			whist.c whist.h

test: # nothing to be done
