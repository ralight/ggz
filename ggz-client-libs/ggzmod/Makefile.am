## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6124 2004-07-17 00:44:02Z josef $

EXTRA_DIST = Doxyfile

lib_LTLIBRARIES = libggzmod.la

libggzmod_la_SOURCES =	ggzmod.c \
			ggzmod.h \
			ggzmod-ggz.h \
			io.c \
			io.h \
			mod.h \
			protocol.h

include_HEADERS = ggzmod.h ggzmod-ggz.h

libggzmod_la_LIBADD = @LDADD@ $(LIB_GGZ)
libggzmod_la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 2:0:1

INCLUDES = $(LIBGGZ_INCLUDES)

#libggz_la_LDFLAGS = -version-info @GGZ_CLIENT_LIBS_VERSION_INFO@
