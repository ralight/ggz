## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3107 2002-01-13 23:41:19Z jdorje $

# ggz_client.h is in this directory
INCLUDES = -I$(top_builddir)/ggzcore

lib_LTLIBRARIES = libggzmod.la

libggzmod_la_SOURCES = ggzmod.c ggzmod.h

include_HEADERS = ggzmod.h

libggzmod_la_LIBADD = @LDADD@

#libggz_la_LDFLAGS = -version-info @GGZ_CLIENT_LIBS_VERSION_INFO@
