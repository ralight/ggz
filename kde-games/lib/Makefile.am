## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

SUBDIRS = .

lib_LTLIBRARIES = libggzkde.la
libggzkde_la_SOURCES = kplayerface.cpp kpfgallery.cpp kgamehelp.cpp
libggzkde_la_LDFLAGS = $(all_libraries)
libggzkde_la_LIBADD = $(LIB_KIO) $(LIB_KDEUI)

noinst_HEADERS = kplayerface.h kpfgallery.h kgamehelp.h

AM_CPPFLAGS = $(all_includes)

METASOURCES = AUTO

