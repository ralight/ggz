## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

SUBDIRS = .

noinst_LIBRARIES = libdot.a

libdot_a_SOURCES = \
	dots.cpp \
	vdots.cpp \
	qdots.cpp

AM_CPPFLAGS = -I$(qt_includes) -I$(kde_includes)

libdot_a_METASOURCES = AUTO

DISTCLEANFILES = $(libdot_a_METASOURCES)

test:
	-g++ -Wall -c test.cpp
	-g++ -o testdots test.o libdot.a

