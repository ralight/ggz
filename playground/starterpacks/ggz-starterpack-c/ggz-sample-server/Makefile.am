# This file contains the server build definitions for Automake.
# You could as well use other build systems like cmake!

ggzdexecmod_PROGRAMS = ggz-sample-server-c

ggz_sample_server_c_SOURCES = \
	main.c \
	game.c \
	game.h \
	net.c \
	net.h

ggz_sample_server_c_CPPFLAGS = $(GGZDMOD_INCLUDES)
ggz_sample_server_c_LDADD = $(LIB_GGZDMOD)

PROTOCOL = $(top_srcdir)/common/ggz-starterpack.protocol
SOURCEFILE = net
include $(top_srcdir)/common/protocol.mk

dist_ggzdgame_DATA = ggz-sample-server.dsc
dist_ggzdroom_DATA = ggz-sample-server.room
