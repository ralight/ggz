SUBDIRS =

noinst_LIBRARIES = libdot.a

libdot_a_SOURCES = \
	dots.cpp \
	vdots.cpp \
	qdots.cpp

#libdot_a_LIBADD = $(all_libraries)

INCLUDES = -I$(qt_includes) -I$(kde_includes)

libdot_a_METASOURCES = AUTO

DISTCLEANFILES = $(libdot_a_METASOURCES)

