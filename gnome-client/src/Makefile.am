## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	$(GNOME_INCLUDEDIR) \
	$(GGZ_CFLAGS) \
	$(LIBGGZ_INCLUDES) \
	$(GGZCORE_INCLUDES)

bin_PROGRAMS = ggz-gnome

ggz_gnome_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	msgboxi.c msgboxi.h \
	msgboxc.c msgboxc.h \
	xtext.c xtext.h \
	login.c login.h \
	ggzclient.c ggzclient.h \
	profilesi.c profilesi.h \
	profilesc.c profilesc.h \
	game.c game.h \
	chat.c chat.h

ggz_gnome_LDADD = $(GNOME_LIBDIR) $(GNOMEUI_LIBS) $(INTLLIBS) $(LDADD) $(GGZ_LIBS) \
	$(LIB_GGZ) $(LIB_GGZCORE)
ggz_gnome_LDFLAGS = $(LIBGGZ_LDFLAGS) $(GGZCORE_LDFLAGS)

