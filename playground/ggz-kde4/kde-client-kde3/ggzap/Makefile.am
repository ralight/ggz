ggzcorepp_includes = -I $(top_srcdir)/ggz-kde/ggzcore++/src
ggzcorepp_libraries = $(top_builddir)/ggz-kde/ggzcore++/src/libggzcorepp.la

bin_PROGRAMS = ggzap

ggzap_SOURCES = \
	main.cpp \
	ggzap.cpp \
	ggzap_tray.cpp \
	ggzap_config.cpp \
	ggzap_handler.cpp \
	ggzap_game.cpp \
	ggzap_gui.cpp \
	ggzap_guialt.cpp \
	ggzap_skin.cpp

noinst_HEADERS = \
	ggzap.h \
	ggzap_tray.h \
	ggzap_config.h \
	ggzap_handler.h \
	ggzap_game.h \
	ggzap_gui.h \
	ggzap_guialt.h \
	ggzap_skin.h

AM_CXXFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES) -ggdb3

ggzap_LDFLAGS = $(all_libraries) $(GGZCORE_LDFLAGS)
ggzap_LDADD = $(ggzcorepp_libraries) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT) $(LIB_GGZCORE)

METASOURCES = AUTO

AM_CPPFLAGS = $(ggzcorepp_includes) $(all_includes)

EXTRA_DIST = README hi16-app-ggzap.png hi32-app-ggzap.png

KDE_ICON = ggzap

pixdir = $(kde_datadir)/kggz/ggzap
dist_pix_DATA = tray.png trayradar.mng redgear.png redgearmask.png

desktopdir = $(datadir)/applications
dist_desktop_DATA = ggzap.desktop

