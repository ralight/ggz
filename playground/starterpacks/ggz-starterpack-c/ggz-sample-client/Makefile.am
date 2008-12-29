EXTRA_DIST = module.dsc.in

ggzexecmod_PROGRAMS = ggz-sample-client-c

ggz_sample_client_c_SOURCES = main.c game.h main_win.c main_win.h net.c net.h
ggz_sample_client_c_CPPFLAGS = $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) $(GGZGTK_INCLUDES) $(GTK_CFLAGS)
ggz_sample_client_c_LDADD = $(LIB_GGZMOD) $(LIB_GGZ) $(LIB_GGZ_GTK) $(GTK_LIBS)
ggz_sample_client_c_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS) $(GGZGTK_LDFLAGS)

include modversion.mk
include ggz.mk

PROTOCOL = $(top_srcdir)/common/ggz_starterpack
SOURCEFILE = net
ROLE = client
include $(top_srcdir)/common/protocol.mk
