ggzexecmod_PROGRAMS = ggzwrap
ggzexecmoddir = $(libdir)/ggz

ggzwrap_SOURCES = ggzwrap.c
ggzwrap_LDADD = $(top_builddir)/ggzmod/libggzmod.la $(LIB_GGZ)
ggzwrap_CPPFLAGS = -I $(top_srcdir)/ggzmod $(LIBGGZ_INCLUDES)

EXTRA_DIST = README.wrap

