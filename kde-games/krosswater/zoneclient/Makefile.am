SUBDIRS = .

lib_LTLIBRARIES = libzoneclient.la

noinst_HEADERS = ZoneGGZModUI.h ZoneGGZ.h ZoneProtocols.h

INCLUDES = -I$(qt_includes) -I$(kde_includes)

libzoneclient_la_LIBADD = @LDADD@ -leasysock

libzoneclient_la_LDFLAGS = -version-info 0:1:0

libzoneclient_la_SOURCES = ZoneGGZModUI.cpp

#libzoneclient_la_METASOURCES = ZoneGGZModUI.moc.cpp
libzoneclient_la_METASOURCES = AUTO
