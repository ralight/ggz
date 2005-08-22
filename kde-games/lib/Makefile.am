SUBDIRS = .

noinst_LTLIBRARIES = libkggzgames.la
libkggzgames_la_SOURCES = kggzseatsdialog.cpp kggzmodule.cpp
libkggzgames_la_LDFLAGS = $(GGZMOD_LDFLAGS) $(all_libraries)
libkggzgames_la_LIBADD = $(LIB_GGZMOD) $(LIB_KIO) $(LIB_KDEUI)

noinst_HEADERS = kggzseatsdialog.h kggzmodule.h

AM_CPPFLAGS = $(GGZMOD_INCLUDES) $(all_includes)

METASOURCES = AUTO

