## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3142 2002-01-19 08:28:37Z bmh $

SUBDIRS = .

ggzdexecmod_PROGRAMS = keepalivesrv

keepalivesrv_SOURCES = main.cpp keepalive.cpp player.cpp world.cpp
# ???
#ggzd_hastings_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a
keepalivesrv_LDADD = $(top_builddir)/ggzdmod/libggzdmod.la

noinst_HEADERS = keepalive.h player.h world.h protocol.h

INCLUDES = -I$(top_srcdir)/ggzdmod

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
