noinst_LIBRARIES = libdot.a

libdot_a_SOURCES = \
	dots.cpp \
	vdots.cpp \
	qdots.cpp

noinst_HEADERS = \
	dots.h \
	vdots.h \
	qdots.h

AM_CPPFLAGS = $(all_includes)

libdot_a_METASOURCES = AUTO

DISTCLEANFILES = $(libdot_a_METASOURCES)

EXTRA_DIST = test.cpp

test:
	-g++ -Wall -c test.cpp
	-g++ -o testdots test.o libdot.a

