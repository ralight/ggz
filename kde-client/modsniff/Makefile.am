SUBDIRS =

bin_PROGRAMS = ggzmodsniff

ggzmodsniff_SOURCES = \
	main.cpp \
	modsniff.c \
	kmodsniff.cpp

easysock_rpath = -rpath $(easysock_libraries)
ggzmodsniff_LDFLAGS = $(easysock_rpath)
ggzmodsniff_LDADD = $(GGZCORE_LDFLAGS) $(LIB_GGZCORE) $(all_libraries) $(KDE_RPATH) $(LIB_KDEUI) -lqt

CFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES)
CXXFLAGS = @COMPILEOPTIONS@

METASOURCES = AUTO

INCLUDES = $(all_includes)

EXTRA_DIST = README

