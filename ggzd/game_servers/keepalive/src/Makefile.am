## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5718 2003-12-18 11:16:43Z dr_maux $

SUBDIRS = .

ggzdexecmod_PROGRAMS = keepalivesrv

keepalivesrv_SOURCES = main.cpp keepalive.cpp player.cpp world.cpp spectator.cpp
keepalivesrv_LDADD = $(top_builddir)/ggzdmod/libggzdmod.la

noinst_HEADERS = keepalive.h player.h world.h protocol.h spectator.h

INCLUDES = -I$(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

test:
	-cp -f $(ggzdexecmod_PROGRAMS) $(top_builddir)/test
