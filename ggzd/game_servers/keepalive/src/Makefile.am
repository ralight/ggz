## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4741 2002-09-26 20:56:08Z dr_maux $

SUBDIRS = .

ggzdexecmod_PROGRAMS = keepalivesrv

keepalivesrv_SOURCES = main.cpp keepalive.cpp player.cpp world.cpp spectator.cpp
keepalivesrv_LDADD = $(top_builddir)/ggzdmod/libggzdmod.la

noinst_HEADERS = keepalive.h player.h world.h protocol.h spectator.h

INCLUDES = -I$(top_srcdir)/ggzdmod

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
