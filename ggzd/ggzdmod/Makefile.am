## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2563 2001-10-14 09:21:40Z jdorje $

SUBDIRS = .

# We prevent parallel execution since libggzdmod.a and libggzdmod.la will
# interfere with each other (they use some of the same files, and make gets
# confused)  --JDS
# .NOTPARALLEL:

# This compiles for static linking with ggzd's game servers
noinst_LIBRARIES = libggzdmod.a
libggzdmod_a_SOURCES =  ggzdmod.h ggzdmod.c

# This compiles for installation as a shared lib for external servers
lib_LTLIBRARIES = libggzdmod.la
libggzdmod_la_SOURCES = ggzdhod.h ggzdmod.c

include_HEADERS = ggzdmod.h
