
SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/intl

bin_PROGRAMS = ggz-gtk

ggzdata_DATA = ggz_gtk_admin.xpm ggz_gtk_guest.xpm ggz_gtk_registered.xpm \
	       ggz_gtk_lag0.xpm ggz_gtk_lag1.xpm ggz_gtk_lag2.xpm ggz_gtk_lag3.xpm \
	       ggz_gtk_lag4.xpm ggz_gtk_lag5.xpm

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
			ggz.c \
			ggz.h \
			info.c \
			info.h \
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
			props.c \
			props.h \
			server.c \
			server.h \
			support.c \
			support.h \
			types.c \
			types.h \
			xtext.c \
			xtext.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_gtk_LDADD = @GTK_LIBS@ $(LDADD) @LIBS@
EXTRA_DIST = $(ggzdata_DATA)
