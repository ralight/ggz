## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2638 2001-11-03 19:47:04Z bmh $

SUBDIRS = .

# This compiles for installation as a shared lib for external servers
lib_LTLIBRARIES = libggzdmod.la
libggzdmod_la_SOURCES = ggzdmod.h ggzdmod.c \
	           	ggz_server.h ggz_server.c \
	                protocol.h

include_HEADERS = ggzdmod.h ggz_server.h

test: # nothing to be done
