## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 2754 2001-11-18 23:58:02Z bmh $

SUBDIRS = .

# We prevent parallel execution since libggzdmod.a and libggzdmod.la will
# interfere with each other (they use some of the same files, and make gets
# confused)  --JDS
.NOTPARALLEL:

# This compiles for static linking with ggzd's game servers
# NOTE: The game servers cannot use the shared library, because it
# won't work with "make test".  Until you find a way around this, don't
# remove the static lib compilation.  --JDS
noinst_LIBRARIES = libggzdmod.a
libggzdmod_a_SOURCES =  ggzdmod.h ggzdmod.c \
			io.c io.h \
			protocol.h \
			ggz_server.h ggz_server.c \
			ggz_stats.h ggz_stats.c

# This compiles for installation as a shared lib for external servers
lib_LTLIBRARIES = libggzdmod.la
libggzdmod_la_LIBADD = @LDADD@
libggzdmod_la_SOURCES = ggzdmod.h ggzdmod.c \
			io.c io.h \
	                protocol.h \
	           	ggz_server.h ggz_server.c \
			ggz_stats.h ggz_stats.c

include_HEADERS = ggzdmod.h ggz_server.h

test: # nothing to be done
