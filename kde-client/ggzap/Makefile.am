SUBDIRS = .

ggzcorepp_includes = -I $(top_srcdir)/ggz-kde/ggzcore++/src
ggz_libraries = $(top_builddir)/ggz-kde/ggzcore++/src/libggzcorepp.la $(GGZCORE_LDFLAGS) $(LIB_GGZCORE)

bin_PROGRAMS = ggzap

ggzap_SOURCES = \
	main.cpp \
	ggzap.cpp \
	ggzap_tray.cpp \
	ggzap_config.cpp \
	ggzap_handler.cpp \
	ggzap_game.cpp \
	ggzap_gui.cpp

noinst_HEADERS = \
	ggzap.h \
	ggzap_tray.h \
	ggzap_config.h \
	ggzap_handler.h \
	ggzap_game.h \
	ggzap_gui.h

CXXFLAGS = @COMPILEOPTIONS@ $(GGZCORE_INCLUDES) -ggdb3

ggzap_LDADD = $(ggz_libraries) $(all_libraries) $(KDE_RPATH) $(LIB_KDEUI) -lqt

METASOURCES = AUTO

INCLUDES = $(ggzcorepp_includes) $(all_includes)

EXTRA_DIST = README

pix_DATA = ggzap.png tray.png trayradar.mng redgear.png redgearmask.png
pixdir = $(kde_datadir)/kggz/ggzap

