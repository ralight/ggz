## Process this file with automake to produce Makefile.in
#  $Id: Makefile.am 5661 2003-10-02 05:50:31Z jdorje $

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/intl \
	$(GGZCORE_INCLUDES) $(LIBGGZ_INCLUDES)

bin_PROGRAMS = ggz-gtk

ggzdata_DATA = ggz_gtk_admin.xpm ggz_gtk_guest.xpm ggz_gtk_registered.xpm \
	       ggz_gtk_lag0.xpm ggz_gtk_lag1.xpm ggz_gtk_lag2.xpm ggz_gtk_lag3.xpm \
	       ggz_gtk_lag4.xpm ggz_gtk_lag5.xpm

ggz_gtk_SOURCES =	main.c \
			about.c \
			about.h \
			about_bg.xpm \
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
			msg_help.xpm \
			msg_info.xpm \
			msg_stop.xpm \
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
			xtext-gtk2.c \
			xtext-gtk2.h

ggz_gtk_CFLAGS = @GTK_CFLAGS@
ggz_gtk_LDADD = @GTK_LIBS@ $(LDADD) @LIBS@ @INTLLIBS@ \
		$(LIB_GGZCORE) $(LIB_GGZ)
ggz_gtk_LDFLAGS = $(GGZCORE_LDFLAGS) $(LIBGGZ_LDFLAGS)

EXTRA_DIST = $(ggzdata_DATA)
