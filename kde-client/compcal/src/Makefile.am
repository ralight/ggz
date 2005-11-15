SUBDIRS = lib

bin_PROGRAMS = compcal

compcal_SOURCES = main.cpp toplevel.cpp app.cpp display.cpp parser.cpp conf.cpp
compcal_LDFLAGS = $(all_libraries)
compcal_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KIO) $(LIB_QT) lib/libcompcal.a
AM_CPPFLAGS = -I$(srcdir)/lib $(all_includes)

noinst_HEADERS = toplevel.h app.h display.h parser.h conf.h

METASOURCES = AUTO

compcaldatadir = $(kde_confdir)
dist_compcaldata_DATA = compcalrc

