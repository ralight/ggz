## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6636 2005-01-11 03:14:32Z jdorje $

SUBDIRS = .

bin_PROGRAMS = keepalivecontrol

keepalivecontrol_SOURCES = main.cpp toplevel.cpp app.cpp connection.cpp create.cpp
keepalivecontrol_LDFLAGS = $(all_libraries)
keepalivecontrol_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)
AM_CPPFLAGS = $(all_includes)

noinst_HEADERS = toplevel.h app.h connection.h create.h

METASOURCES = AUTO

