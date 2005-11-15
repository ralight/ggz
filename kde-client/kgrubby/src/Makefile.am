bin_PROGRAMS = kgrubby

kgrubby_SOURCES = main.cpp toplevel.cpp app.cpp plugin.cpp
kgrubby_LDFLAGS = $(all_libraries)
kgrubby_LDADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KIO) $(LIB_QT)
AM_CPPFLAGS = $(all_includes)

noinst_HEADERS = toplevel.h app.h plugin.h

METASOURCES = AUTO

