## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 4672 2002-09-23 18:54:24Z jdorje $

SUBDIRS = lib

bin_PROGRAMS = compcal

compcal_SOURCES = main.cpp toplevel.cpp app.cpp display.cpp parser.cpp
compcal_LDFLAGS = $(all_libraries)
compcal_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KIO) $(LIB_QT) lib/libcompcal.a
INCLUDES = -I$(srcdir)/lib $(all_includes)

noinst_HEADERS = toplevel.h app.h display.h parser.h

METASOURCES = AUTO
