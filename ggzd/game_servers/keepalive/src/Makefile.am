## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6258 2004-11-04 23:00:59Z jdorje $

SUBDIRS = .

ggzdexecmod_PROGRAMS = keepalivesrv

keepalivesrv_SOURCES = main.cpp keepalive.cpp player.cpp world.cpp spectator.cpp
keepalivesrv_LDADD = $(top_builddir)/ggzdmod/libggzdmod.la

noinst_HEADERS = keepalive.h player.h world.h protocol.h spectator.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
