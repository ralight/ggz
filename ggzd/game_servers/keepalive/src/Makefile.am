## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4219 2002-05-20 19:52:10Z dr_maux $

SUBDIRS = .

ggzdexecmod_PROGRAMS = keepalivesrv

keepalivesrv_SOURCES = main.cpp keepalive.cpp player.cpp world.cpp spectator.cpp
# ???
#ggzd_hastings_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a
keepalivesrv_LDADD = $(top_builddir)/ggzdmod/libggzdmod.la

noinst_HEADERS = keepalive.h player.h world.h protocol.h spectator.h

INCLUDES = -I$(top_srcdir)/ggzdmod

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
