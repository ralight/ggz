SUBDIRS =

noinst_LIBRARIES = libdot.a

libdot_a_SOURCES = \
	dots.cpp \
	vdots.cpp \
	qdots.cpp

INCLUDES = -I$(qt_includes) -I$(kde_includes)

libdot_a_METASOURCES = AUTO

DISTCLEANFILES = $(libdot_a_METASOURCES)

test:
	-g++ -Wall -c test.cpp
	-g++ -o testdots test.o libdot.a

