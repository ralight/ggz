noinst_LTLIBRARIES = libzoneclient.la

libzoneclient_la_LIBADD = $(LIB_GGZ) $(LIB_GGZMOD) $(LIB_KDEUI)
libzoneclient_la_LDFLAGS = $(LIBGGZ_LDFLAGS) $(GGZMOD_LDFLAGS) $(all_libraries) -version-info 1:0:0
libzoneclient_la_SOURCES = ZoneGGZModUI.cpp

AM_CPPFLAGS = $(LIBGGZ_INCLUDES) $(GGZMOD_INCLUDES) $(all_includes)

noinst_HEADERS = ZoneGGZModUI.h ZoneGGZ.h

METASOURCES = AUTO

