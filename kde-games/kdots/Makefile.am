SUBDIRS = Dot .

bindir = $(ggzexecmoddir)

bin_PROGRAMS = kdots_client

kdots_client_SOURCES = \
	main.cpp \
	kdots.cpp \
	kdots_about.cpp \
	kdots_options.cpp \
	kdots_help.cpp

noinst_HEADERS = \
	kdots.h \
	kdots_about.h \
	kdots_options.h \
	kdots_help.h

INCLUDES = $(EASYSOCK_INCLUDES) $(all_includes) -I Dot

kdots_client_LDADD = $(all_libraries) -lqt -lkdeui -lkdecore -L Dot -ldot

METASOURCES = AUTO

EXTRA_DIST = module.dsc.in
