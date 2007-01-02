SUBDIRS = ai

ggzexecmod_PROGRAMS = muehle

muehle_SOURCES = main.cpp toplevel.cpp board.cpp stone.cpp qweb.cpp qextpainter.cpp net.cpp qwebpath.cpp
muehle_LDFLAGS = $(all_libraries)
muehle_LDADD = $(top_builddir)/lib/libkggzgames.la $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF)
AM_CPPFLAGS = -I $(top_srcdir)/lib $(all_includes)

noinst_HEADERS = toplevel.h board.h stone.h qweb.h qextpainter.h net.h qwebpath.h

METASOURCES = AUTO

dist_kde_conf_DATA = muehlerc

