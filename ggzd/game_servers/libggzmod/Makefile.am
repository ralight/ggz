## Process this file with automake to produce Makefile.in

SUBDIRS = .

# Problem: ideally, libggzdmod would be compiled and installed as a shared library.
# We weren't sure how to do this with the current setup, because the ggzd game
# servers need to link to it.  Since they're in the same package and are compiled/
# installed at the same time, it's a problem.  The "solution" here is to statically
# link the ggzd game servers with libggzdmod.a and also install the shared libraries
# to be linked to by external game servers.    --JDS

# This compiles for static linking with ggzd's game servers
noinst_LIBRARIES = libggzdmod.a
libggzdmod_a_SOURCES = ggz.c ggz_server.h ggz_protocols.h

# This compiles for installation as a shared lib for external servers
lib_LTLIBRARIES = libggzdmod.la
libggzdmod_la_SOURCES = ggz.c ggz_server.h ggz_protocols.h
include_HEADERS = ggz_server.h ggz_protocols.h
