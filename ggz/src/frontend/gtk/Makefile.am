## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 6391 2004-11-16 07:15:24Z jdorje $

SUBDIRS = .

AM_CPPFLAGS = @GTK_CFLAGS@ -I$(top_srcdir)/intl \
	$(GGZCORE_INCLUDES) $(GGZMOD_INCLUDES) $(LIBGGZ_INCLUDES)

bin_PROGRAMS = ggz-gtk

ggzdata_DATA =		about_bg.png		\
			ggz_gtk_admin.png 	\
			ggz_gtk_guest.png 	\
			ggz_gtk_registered.png	\
			ggz_gtk_lag0.svg 	\
			ggz_gtk_lag1.svg 	\
			ggz_gtk_lag2.svg	\
			ggz_gtk_lag3.svg	\
			ggz_gtk_lag4.svg	\
			ggz_gtk_lag5.svg

ggz_gtk_SOURCES =	main.c \
			about.c \
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
			xtext.h

ggz_gtk_LDADD = @GTK_LIBS@ $(LDADD) @LIBS@ @INTLLIBS@ \
		$(LIB_GGZCORE) $(LIB_GGZMOD) $(LIB_GGZ)
ggz_gtk_LDFLAGS = $(GGZCORE_LDFLAGS) $(GGZMOD_LDFLAGS) $(LIBGGZ_LDFLAGS)

EXTRA_DIST = $(ggzdata_DATA) xtext.README
