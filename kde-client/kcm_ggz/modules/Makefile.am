kde_module_LTLIBRARIES = libkcm_ggz_default.la libkcm_ggz_metaserver.la libkcm_ggz_ggzd.la libkcm_ggz_games.la
noinst_LTLIBRARIES = libkcm_ggz_drs.la

libkcm_ggz_default_la_SOURCES = kcm_ggz_default.cpp
libkcm_ggz_default_la_LIBADD = $(LIB_KDEUI) $(LIB_KDECORE)
libkcm_ggz_default_la_METASOURCES = kcm_ggz_default.moc
#kcm_ggz_pane.moc

libkcm_ggz_metaserver_la_SOURCES = kcm_ggz_metaserver.cpp metaserver_add.cpp metaserver_add_server.cpp
libkcm_ggz_metaserver_la_LIBADD = $(LIB_KDEUI) $(LIB_KDECORE)
libkcm_ggz_metaserver_la_METASOURCES = kcm_ggz_metaserver.moc metaserver_add.moc metaserver_add_server.moc
#kcm_ggz_pane.moc

libkcm_ggz_drs_la_SOURCES = kcm_ggz_drs.cpp
libkcm_ggz_drs_la_LIBADD = $(LIB_KDEUI) $(LIB_KDECORE)
libkcm_ggz_drs_la_METASOURCES = kcm_ggz_drs.moc

libkcm_ggz_ggzd_la_SOURCES = kcm_ggz_ggzd.cpp
libkcm_ggz_ggzd_la_LIBADD = $(LIB_KDEUI) $(LIB_KDECORE)
libkcm_ggz_ggzd_la_METASOURCES = kcm_ggz_ggzd.moc

libkcm_ggz_games_la_SOURCES = kcm_ggz_games.cpp
libkcm_ggz_games_la_LIBADD = $(LIB_KDEUI) $(LIB_KDECORE)
libkcm_ggz_games_la_METASOURCES = kcm_ggz_games.moc

AM_CPPFLAGS = -I$(qt_includes) -I$(kde_includes) -I$(top_srcdir)/kcm_ggz
AM_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)

noinst_HEADERS = kcm_ggz_default.h kcm_ggz_metaserver.h kcm_ggz_drs.h kcm_ggz_ggzd.h kcm_ggz_games.h metaserver_add.h metaserver_add_server.h

gamedir = $(kde_datadir)/kggz/kcmggz/icons
dist_game_DATA = game.png game_kde.png game_gnome.png game_sdl.png game_console.png game_grubby.png game_x11.png game_java.png gear.png

