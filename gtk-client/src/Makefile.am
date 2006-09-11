AM_CPPFLAGS = $(GTK_CFLAGS) -I$(top_srcdir)/intl \
	$(GGZCORE_INCLUDES) $(LIBGGZ_INCLUDES) \
	-DGGZGTKDATADIR=\"$(ggzgtkdatadir)\" \
	-I$(top_srcdir)/ggz-gtk

bin_PROGRAMS = ggz-gtk

ggz_gtk_SOURCES = main.c

ggz_gtk_DEPENDENCIES = $(top_builddir)/ggz-gtk/libggz-gtk.la
ggz_gtk_LDADD = $(top_builddir)/ggz-gtk/libggz-gtk.la
