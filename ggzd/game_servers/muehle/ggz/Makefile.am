## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6258 2004-11-04 23:00:59Z jdorje $

SUBDIRS = .

noinst_LIBRARIES = libggzgameserver.a
libggzgameserver_a_SOURCES = ggzgameserver.cpp
noinst_HEADERS = ggzgameserver.h
AM_CPPFLAGS = -I $(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

test: # avoid warning
