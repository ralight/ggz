SUBDIRS = Dot .

ggzexecmod_PROGRAMS = kdots_client

kdots_client_SOURCES = \
	main.cpp \
	kdots.cpp \
	kdots_about.cpp \
	kdots_options.cpp \
	kdots_help.cpp \
	kdots_proto.cpp \
	kdots_replay.cpp \
	kdots_win.cpp

noinst_HEADERS = \
	kdots.h \
	kdots_about.h \
	kdots_options.h \
	kdots_help.h \
	kdots_proto.h \
	kdots_replay.h \
	kdots_win.h

AM_CPPFLAGS = -I $(top_srcdir)/kdots/Dot -I $(top_srcdir)/lib $(all_includes) $(LIBGGZ_INCLUDES) $(GGZMOD_INCLUDES)

kdots_client_LDFLAGS = $(all_libraries) $(LIBGGZ_LDFLAGS)
kdots_client_LDADD = Dot/libdot.a $(top_builddir)/lib/libkggzgames.la $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_GGZ) $(LIB_GGZMOD)

METASOURCES = AUTO

KDE_ICON = kdots

gfxdir = $(ggzdatadir)/kdots
dist_gfx_DATA = dragon.png firedragon.png hi32-app-kdots.png snowdragon.png

EXTRA_DIST = \
	hi32-app-kdots.png

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force
 
uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc
