SUBDIRS = .

lib_LTLIBRARIES = libzoneserver.la

include_HEADERS = ZoneGGZModServer.h ZoneGGZModGGZ.h ZoneProtocols.h

libzoneserver_la_LIBADD = @LDADD@

libzoneserver_la_LDFLAGS = -version-info 0:1:0

libzoneserver_la_SOURCES = ZoneGGZModGGZ.cpp \
			   ZoneGGZModServer.cpp
