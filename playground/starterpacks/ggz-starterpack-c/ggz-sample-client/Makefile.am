AM_CPPFLAGS = -I$(srcdir)/../common $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES) \
		$(GTK_CFLAGS)

EXTRA_DIST = module.dsc.in

ggzexecmod_PROGRAMS = ggz-sample-client-c

ggz_sample_client_c_SOURCES =			\
	game.h				\
	main.c				\
	main_win.c	main_win.h	\
	support.c	support.h	\
	net.c		net.h

ggz_sample_client_c_LDADD =	$(builddir)/../common/libgtk-games.a			\
		$(GTK_LIBS) $(INTLLIBS) $(LDADD) \
		$(LIB_GGZMOD) $(LIB_GGZ)
ggz_sample_client_c_LDFLAGS = $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

include modversion.mk
include ggz.mk

PROTOCOL = $(top_srcdir)/common/ggz-starterpack.protocol
SOURCEFILE = net
include $(top_srcdir)/common/protocol.mk
