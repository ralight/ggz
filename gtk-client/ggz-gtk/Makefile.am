ggzgtkdatadir=$(datadir)/ggz/$(PACKAGE)
AM_CPPFLAGS = $(GTK_CFLAGS) -I$(top_srcdir)/intl \
	$(GGZCORE_INCLUDES) $(LIBGGZ_INCLUDES) \
	-DGGZGTKDATADIR=\"$(ggzgtkdatadir)\"

dist_ggzgtkdata_DATA =	about_bg.png		\
			ggz_gtk_admin.svg 	\
			ggz_gtk_guest.svg 	\
			ggz_gtk_host.svg	\
			ggz_gtk_registered.svg	\
			ggz_gtk_bot.svg		\
			ggz_gtk_lag0.svg 	\
			ggz_gtk_lag1.svg 	\
			ggz_gtk_lag2.svg	\
			ggz_gtk_lag3.svg	\
			ggz_gtk_lag4.svg	\
			ggz_gtk_lag5.svg

if INSTALL_GGZ_GTK
lib_LTLIBRARIES = libggz-gtk.la

include_HEADERS = ggz-embed.h ggz-gtk.h
else
noinst_LTLIBRARIES = libggz-gtk.la
endif

libggz_gtk_la_SOURCES =	about.c \
			about.h \
			chat.c \
			chat.h \
			client.c \
			client.h \
			first.c \
			first.h \
			game.c \
			game.h \
			ggzclient.c \
			ggzclient.h \
			ggzutils.c	ggzutils.h	\
			ggz-embed.h		\
			ggz-gtk.h		\
			launch.c \
			launch.h \
			license.c \
			license.h \
			login.c \
			login.h \
			motd.c \
			motd.h \
			msgbox.c \
			msgbox.h \
			pick_module.c \
			pick_module.h \
			playerinfo.c \
			playerinfo.h \
			playerlist.c \
			playerlist.h \
			props.c \
			props.h \
			roominfo.c \
			roominfo.h \
			roomlist.c		\
			roomlist.h		\
			server.c \
			server.h \
			support.c \
			support.h \
			tablelist.c \
			tablelist.h \
			types.c \
			types.h \
			xtext.c \
			xtext.h \
			xtext-ggz.h

libggz_gtk_la_LIBADD = $(GTK_LIBS) $(LDADD) $(LIBS) $(INTLLIBS) \
			$(LIB_GGZCORE) $(LIB_GGZ) $(LIB_GETTEXT)
libggz_gtk_la_LDFLAGS = $(GGZCORE_LDFLAGS) $(LIBGGZ_LDFLAGS) -version-info 2:0:0

if SYMBOL_VISIBILITY
libggz_gtk_la_LDFLAGS += -Wl,--version-script=$(srcdir)/linker.map
libggz_gtk_la_DEPENDENCIES = $(srcdir)/linker.map
endif

#if WIN32
#ggz_gtk_SOURCES += ggz-gtk-icon.rc
#ggz_gtk_LDADD += ggz-gtk-icon.o
#ggz_gtk_DEPENDENCIES += ggz-gtk-icon.o
#ggz-gtk-icon.o: ggz-gtk-icon.rc
#	i586-mingw32msvc-windres -i $< $@
#endif

EXTRA_DIST = linker.map
