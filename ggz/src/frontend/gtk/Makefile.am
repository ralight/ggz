
SUBDIRS = .

INCLUDES = @GTK_CFLAGS@ -I$(top_srcdir)/src/ggzcore -I$(top_srcdir)/intl @EASYSOCK_INC@

bin_PROGRAMS = ggz-gtk

ggz_gtk_SOURCES =	main.c \
			about.c \
			about.h \
			chat.c \
			chat.h \
			client.c \
			client.h \
			ggz.c \
			ggz.h \
			launch.c \
			launch.h \
			license.c \
			license.h \
			login.c \
			login.h \
			motd.c \
			motd.h \
			props.c \
			props.h \
			support.c \
			support.h \
			xtext.c \
			xtext.h

CFLAGS = @GTK_CFLAGS@ @CFLAGS@
ggz_gtk_LDADD = @GTK_LIBS@ $(LDADD) $(top_builddir)/src/ggzcore/libggzcore.a @LIBS@ @EASYSOCK_LIB@
