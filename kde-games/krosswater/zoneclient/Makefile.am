SUBDIRS = .

lib_LTLIBRARIES = libzoneclient.la

libzoneclient_la_LIBADD = -lqt $(LIB_EASYSOCK)
libzoneclient_la_LDFLAGS = $(qt_libraries) $(EASYSOCK_LDFLAGS) -version-info 0:1:0
libzoneclient_la_SOURCES = ZoneGGZModUI.cpp

INCLUDES = $(all_includes) $(EASYSOCK_INCLUDES)

noinst_HEADERS = ZoneGGZModUI.h ZoneGGZ.h

METASOURCES = AUTO

