SUBDIRS = .

EXTRA_DIST = DESIGN tictactoe.protocol

bin_PROGRAMS = ggzcommgen

ggzcommgen_SOURCES = main.c ggzcommgen.c
ggzcommgen_LDADD = $(top_builddir)/metaserv/minidom/libminidom.a

noinst_HEADERS = ggzcommgen.h

INCLUDES = -I $(top_srcdir)/metaserv/minidom

