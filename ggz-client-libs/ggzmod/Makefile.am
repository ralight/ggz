## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4597 2002-09-17 02:40:57Z jdorje $

EXTRA_DIST = Doxyfile

lib_LTLIBRARIES = libggzmod.la

libggzmod_la_SOURCES =	ggzmod.c \
			ggzmod.h \
			io.c \
			io.h \
			mod.h \
			protocol.h

include_HEADERS = ggzmod.h

libggzmod_la_LIBADD = @LDADD@

#libggz_la_LDFLAGS = -version-info @GGZ_CLIENT_LIBS_VERSION_INFO@
