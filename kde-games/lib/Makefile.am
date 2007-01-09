SUBDIRS = kggzmod

noinst_LTLIBRARIES = libkggzgames.la
libkggzgames_la_SOURCES = kggzseatsdialog.cpp qsvgpixmap.cpp kggzsystem.cpp kggzpacket.cpp
libkggzgames_la_LDFLAGS = $(GGZMOD_LDFLAGS) $(RSVG_LDFLAGS) $(all_libraries)
libkggzgames_la_LIBADD = $(LIB_KIO) $(LIB_KDEUI)

noinst_HEADERS = kggzseatsdialog.h qsvgpixmap.h kggzsystem.h kggzpacket.h

AM_CPPFLAGS = $(RSVG_INCLUDES) $(all_includes) -DPREFIX=\"$(prefix)\"

METASOURCES = AUTO

