SUBDIRS = .

lib_LTLIBRARIES = libzoneclient.la

libzoneclient_la_LIBADD = $(LIB_KDEUI) $(LIB_QT) $(LIB_GGZ)
# note: qt_libraries does not work (bug in kde's build system)
libzoneclient_la_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS) -version-info 0:1:0
libzoneclient_la_SOURCES = ZoneGGZModUI.cpp

INCLUDES = $(all_includes) $(LIBGGZ_INCLUDES)

noinst_HEADERS = ZoneGGZModUI.h ZoneGGZ.h

METASOURCES = AUTO

