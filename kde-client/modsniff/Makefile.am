SUBDIRS =

bin_PROGRAMS = ggzmodsniff

ggzmodsniff_SOURCES = \
	main.cpp \
	modsniff.c \
	kmodsniff.cpp

ggzmodsniff_LDADD = $(LIB_GGZCORE) $(all_libraries) $(KDE_RPATH) $(LIB_KDEUI)

CFLAGS = @COMPILEOPTIONS@
CXXFLAGS = @COMPILEOPTIONS@

METASOURCES = AUTO

INCLUDES = $(all_includes)

EXTRA_DIST = README

