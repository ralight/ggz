## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3880 2002-04-10 13:53:51Z dr_maux $

SUBDIRS = .

noinst_LIBRARIES = libggzgameserver.a
libggzgameserver_a_SOURCES = ggzgameserver.cpp
noinst_HEADERS = ggzgameserver.h
INCLUDES = -I $(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

test: # avoid warning
