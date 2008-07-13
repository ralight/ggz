ggzexecmod_PROGRAMS = kconnectx

kconnectx_SOURCES = \
	main.cpp \
	kconnectx.cpp \
	win.cpp \
	proto.cpp \
	opt.cpp

noinst_HEADERS = \
	kconnectx.h \
	win.h \
	proto.h \
	opt.h

AM_CPPFLAGS = -I $(top_srcdir)/lib $(all_includes)

kconnectx_LDFLAGS = $(LIBGGZ_LDFLAGS) $(LIBGGZMOD_LDFLAGS) $(all_libraries)
kconnectx_LDADD = $(top_builddir)/lib/libkggzgames.la $(top_builddir)/lib/kggzmod/libkggzmod.la $(LIB_KDECORE) $(LIB_KDEUI)

METASOURCES = AUTO

pixmapdir = $(kde_datadir)/kconnectx
dist_pixmap_DATA = red.svg.png yellow.svg.png red.svg yellow.svg

KDE_ICON = kconnectx

EXTRA_DIST = \
	hi16-app-kconnectx.png \
	hi32-app-kconnectx.png \
	hi48-app-kconnectx.png

install-data-local:
	$(GGZ_CONFIG) -D --install --modfile=module.dsc --force

uninstall-local:
	-$(GGZ_CONFIG) -D --remove --modfile=module.dsc

