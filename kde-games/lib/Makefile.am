SUBDIRS = .

lib_LTLIBRARIES = libggzkde.la
libggzkde_la_SOURCES = kggzseatsdialog.cpp kggzmodule.cpp
libggzkde_la_LDFLAGS = $(GGZMOD_LDFLAGS) $(all_libraries)
libggzkde_la_LIBADD = $(LIB_GGZMOD) $(LIB_KIO) $(LIB_KDEUI)

noinst_HEADERS = kggzseatsdialog.h kggzmodule.h

AM_CPPFLAGS = $(GGZMOD_INCLUDES) $(all_includes)

METASOURCES = AUTO

