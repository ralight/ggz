## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5505 2003-05-09 21:30:18Z dr_maux $

SUBDIRS = lib

bin_PROGRAMS = compcal

compcal_SOURCES = main.cpp toplevel.cpp app.cpp display.cpp parser.cpp conf.cpp
compcal_LDFLAGS = $(all_libraries)
compcal_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KIO) $(LIB_QT) lib/libcompcal.a
INCLUDES = -I$(srcdir)/lib $(all_includes)

noinst_HEADERS = toplevel.h app.h display.h parser.h conf.h

METASOURCES = AUTO

compcaldatadir = $(kde_confdir)
compcaldata_DATA = compcalrc

