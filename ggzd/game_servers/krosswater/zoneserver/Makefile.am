SUBDIRS = .

INCLUDES = -I $(top_srcdir)/ggzdmod $(LIBGGZ_INCLUDES)

noinst_LTLIBRARIES = libzoneserver.la

libzoneserver_la_LIBADD = @LDADD@
libzoneserver_la_LDFLAGS = -version-info 0:1:0
libzoneserver_la_SOURCES = ZoneGGZModServer.cpp

noinst_HEADERS = ZoneGGZ.h ZoneGGZModServer.h

