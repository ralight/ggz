## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2393 2001-09-08 01:46:47Z jdorje $

SUBDIRS = .

# We prevent parallel execution since libggzdmod.a and libggzdmod.la will
# interfere with each other (they use some of the same files, and make gets
# confused)  --JDS
.NOTPARALLEL:

# Problem: ideally, libggzdmod would be compiled and installed as a shared library.
# We weren't sure how to do this with the current setup, because the ggzd game
# servers need to link to it.  Since they're in the same package and are compiled/
# installed at the same time, it's a problem.  The "solution" here is to statically
# link the ggzd game servers with libggzdmod.a and also install the shared libraries
# to be linked to by external game servers.    --JDS

# This compiles for static linking with ggzd's game servers
noinst_LIBRARIES = libggzdmod.a
libggzdmod_a_SOURCES =  ggz_protocols.h\
			ggz.c ggz_server.h \
			ggz_stats.h ggz_stats.c

# This compiles for installation as a shared lib for external servers
lib_LTLIBRARIES = libggzdmod.la
libggzdmod_la_SOURCES = ggz_protocols.h \
			ggz.c ggz_server.h \
			ggz_stats.h ggz_stats.c
include_HEADERS = ggz_server.h ggz_stats.h

test: # nothing to be done
