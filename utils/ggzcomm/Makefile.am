SUBDIRS = .

EXTRA_DIST = DESIGN tictactoe.protocol

bin_PROGRAMS = ggzcommgen

ggzcommgen_SOURCES = main.c ggzcommgen.c
ggzcommgen_LDADD = $(top_builddir)/metaserv/minidom/libminidom.a

noinst_HEADERS = ggzcommgen.h

AM_CPPFLAGS = -I $(top_srcdir)/metaserv/minidom

test:
	$(top_builddir)/ggzcomm/ggzcommgen --protocol=tictactoe --format=c --task=server
	$(top_builddir)/ggzcomm/ggzcommgen --protocol=ggzcards --format=c++ --task=client

