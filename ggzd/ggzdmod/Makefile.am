## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6960 2005-03-04 01:13:04Z josef $

SUBDIRS = .

EXTRA_DIST = Doxyfile

# We prevent parallel execution since libggzdmod.a and libggzdmod.la will
# interfere with each other (they use some of the same files, and make gets
# confused).  This is only necessary if both libs are being compiled.  --JDS
#.NOTPARALLEL:

# This compiles for installation as a shared lib for external servers
lib_LTLIBRARIES = libggzdmod.la

libggzdmod_la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 3:0:0
libggzdmod_la_LIBADD = $(LIB_GGZ)
libggzdmod_la_SOURCES = ggzdmod.h ggzdmod-ggz.h \
			ggzdmod.c \
			io.c io.h \
			protocol.h mod.h

# This compiles for static linking with ggzd's game servers.  On some systems
# the game servers cannot use the shared library, because it won't work with
# "make test".  We're ignoring this problem for now until we find a
# better workaround.  --JDS
#noinst_LIBRARIES = libggzdmod.a
#libggzdmod_a_SOURCES = $(libggzdmod_la_SOURCES)

include_HEADERS = ggzdmod.h ggzdmod-ggz.h

AM_CPPFLAGS = $(LIBGGZ_INCLUDES)
