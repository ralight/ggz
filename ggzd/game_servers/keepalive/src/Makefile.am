## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6959 2005-03-04 00:18:42Z josef $

SUBDIRS = .

ggzdexecmod_PROGRAMS = keepalivesrv

keepalivesrv_SOURCES = main.cpp keepalive.cpp player.cpp world.cpp spectator.cpp
keepalivesrv_LDADD = $(top_builddir)/ggzdmod/libggzdmod.la

noinst_HEADERS = keepalive.h player.h world.h protocol.h spectator.h

AM_CPPFLAGS = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

