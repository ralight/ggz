## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4264 2002-06-22 05:10:28Z bmh $

EXTRA_DIST = Doxyfile

lib_LTLIBRARIES = libggzmod.la

libggzmod_la_SOURCES = ggzmod.c ggzmod.h \
		       io.c     io.h

include_HEADERS = ggzmod.h

libggzmod_la_LIBADD = @LDADD@

#libggz_la_LDFLAGS = -version-info @GGZ_CLIENT_LIBS_VERSION_INFO@
