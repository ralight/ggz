SUBDIRS = .

bin_PROGRAMS = ggzmodsniff

ggzmodsniff_SOURCES = \
	main.cpp \
	modsniff.c \
	kmodsniff.cpp

ggzmodsniff_LDFLAGS = $(all_libraries)
ggzmodsniff_LDADD = $(GGZCORE_LDFLAGS) $(LIB_GGZCORE) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)

CFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES)
CXXFLAGS = @COMPILEOPTIONS@

METASOURCES = AUTO

INCLUDES = $(all_includes)

EXTRA_DIST = README

