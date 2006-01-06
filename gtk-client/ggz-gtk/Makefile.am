## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 7736 2006-01-06 08:28:12Z jdorje $

SUBDIRS = .

AM_CPPFLAGS = @GTK_CFLAGS@ -I$(top_srcdir)/intl \
	$(GGZCORE_INCLUDES) $(LIBGGZ_INCLUDES)

bin_PROGRAMS = ggz-gtk
noinst_LIBRARIES = libggz-gtk.a

ggzdata_DATA =		about_bg.png		\
			ggz_gtk_admin.svg 	\
			ggz_gtk_guest.svg 	\
			ggz_gtk_registered.svg	\
			ggz_gtk_bot.svg		\
			ggz_gtk_lag0.svg 	\
			ggz_gtk_lag1.svg 	\
			ggz_gtk_lag2.svg	\
			ggz_gtk_lag3.svg	\
			ggz_gtk_lag4.svg	\
			ggz_gtk_lag5.svg

libggz_gtk_a_SOURCES =	about.c \
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

ggz_gtk_SOURCES = 	main.c

ggz_gtk_DEPENDENCIES = libggz-gtk.a
ggz_gtk_LDADD = libggz-gtk.a @GTK_LIBS@ $(LDADD) @LIBS@ @INTLLIBS@ \
		$(LIB_GGZCORE) $(LIB_GGZ)
ggz_gtk_LDFLAGS = $(GGZCORE_LDFLAGS) $(LIBGGZ_LDFLAGS)

EXTRA_DIST = $(ggzdata_DATA)
