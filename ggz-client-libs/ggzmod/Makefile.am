## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6866 2005-01-24 01:39:48Z jdorje $

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

# We should libadd ggzcore here, but that would make a circular dependency.
# Instead it is added inside AC_GGZ_GGZMOD.
libggzmod_la_LIBADD = $(LIB_GGZ)
libggzmod_la_LDFLAGS = $(LIBGGZ_LDFLAGS) -version-info 3:0:0

AM_CPPFLAGS = $(LIBGGZ_INCLUDES) -I$(top_srcdir)/ggzcore

#libggz_la_LDFLAGS = -version-info @GGZ_CLIENT_LIBS_VERSION_INFO@
