SUBDIRS = .

ggzdexecmod_PROGRAMS = keepalivesrv

keepalivesrv_SOURCES = main.cpp keepalive.cpp player.cpp world.cpp
# ???
#ggzd_hastings_DEPENDENCIES = $(top_builddir)/ggzdmod/libggzdmod.a
keepalivesrv_LDFLAGS = $(EASYSOCK_LDFLAGS)
keepalivesrv_LDADD = $(top_builddir)/ggzdmod/libggzdmod.la $(LIB_EASYSOCK)

noinst_HEADERS = keepalive.h player.h world.h protocol.h

INCLUDES = -I$(top_srcdir)/ggzdmod $(EASYSOCK_INCLUDES)

