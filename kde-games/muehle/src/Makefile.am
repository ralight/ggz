## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

SUBDIRS = ggzwrap ai

ggzexecmod_PROGRAMS = muehle

muehle_SOURCES = main.cpp toplevel.cpp board.cpp stone.cpp qweb.cpp qextpainter.cpp net.cpp qwebpath.cpp
muehle_LDFLAGS = $(all_libraries)
muehle_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(KNEWSTUFF)
#muehle_CXXFLAGS = -ggdb3
AM_CPPFLAGS = $(all_includes)

noinst_HEADERS = toplevel.h board.h stone.h qweb.h qextpainter.h net.h qwebpath.h

METASOURCES = AUTO

