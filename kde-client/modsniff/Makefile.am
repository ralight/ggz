## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

SUBDIRS = .

bin_PROGRAMS = ggzmodsniff

ggzmodsniff_SOURCES = \
	main.cpp \
	modsniff.c \
	kmodsniff.cpp

ggzmodsniff_LDFLAGS = $(all_libraries)
ggzmodsniff_LDADD = $(GGZCORE_LDFLAGS) $(LIB_GGZCORE) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)

AM_CFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES)
AM_CXXFLAGS = @COMPILEOPTIONS@

METASOURCES = AUTO

AM_CPPFLAGS = $(all_includes)

EXTRA_DIST = README

