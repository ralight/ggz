SUBDIRS = kggzmod

noinst_LTLIBRARIES = libkggzgames.la
libkggzgames_la_SOURCES = kggzseatsdialog.cpp qsvgpixmap.cpp
libkggzgames_la_LDFLAGS = $(GGZMOD_LDFLAGS) $(RSVG_LDFLAGS) $(all_libraries)
libkggzgames_la_LIBADD = $(LIB_GGZMOD) $(LIB_KIO) $(LIB_KDEUI)

noinst_HEADERS = kggzseatsdialog.h qsvgpixmap.h

AM_CPPFLAGS = $(GGZMOD_INCLUDES) $(RSVG_INCLUDES) $(all_includes)

METASOURCES = AUTO

