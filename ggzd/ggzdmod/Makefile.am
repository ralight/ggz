## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6068 2004-07-09 13:14:00Z josef $

SUBDIRS = .

EXTRA_DIST = Doxyfile

# We prevent parallel execution since libggzdmod.a and libggzdmod.la will
# interfere with each other (they use some of the same files, and make gets
# confused).  This is only necessary if both libs are being compiled.  --JDS
#.NOTPARALLEL:

# This compiles for installation as a shared lib for external servers
lib_LTLIBRARIES = libggzdmod.la

libggzdmod_la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 2:1:0
libggzdmod_la_LIBADD = @LDADD@ $(LIB_GGZ)
libggzdmod_la_SOURCES = ggzdmod.h ggzdmod-ggz.h \
			ggzdmod.c \
			io.c io.h \
			protocol.h mod.h \
			\
			elo.h elo.c \
			ggz_stats.h ggz_stats.c

# This compiles for static linking with ggzd's game servers.  On some systems
# the game servers cannot use the shared library, because it won't work with
# "make test".  We're ignoring this problem for now until we find a
# better workaround.  --JDS
#noinst_LIBRARIES = libggzdmod.a
#libggzdmod_a_SOURCES = $(libggzdmod_la_SOURCES)

include_HEADERS = ggzdmod.h ggzdmod-ggz.h ggz_stats.h

INCLUDES = $(LIBGGZ_INCLUDES)

test: # nothing to be done
