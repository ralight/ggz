bin_PROGRAMS = kggz
INCLUDES = $(all_includes)
LDFLAGS  = $(all_libraries) $(KDE_RPATH)
#kggz_LDADD = $(LIB_KFILE)

kggz_SOURCES = kggz_grubby.cpp \
	kggz_install.cpp \
	kggz_playerinfo.cpp \
	kggz_profiles.cpp \
	kggz_hosts.cpp \
	kggz_ftp.cpp \
	kggz.cpp \
	kggz_update.cpp \
	kggz_roomsmenu.cpp \
	kggz_browser.cpp \
	kggz_state.cpp \
	kggz_chat.cpp \
	kggz_startup.cpp \
	kggz_tables.cpp \
	kggz_users.cpp \
	kggz_chatwidget.cpp \
	kggz_motd.cpp \
	kggz_teams.cpp \
	kggz_preferences.cpp \
	kggz_games.cpp \
	kggz_settings.cpp \
	kggz_connect.cpp \
	kggz_launch.cpp \
	kggz_server.cpp \
	main.cpp

METASOURCES = kggz_grubby.moc.cpp \
	kggz_install.moc.cpp \
	kggz_playerinfo.moc.cpp \
	kggz_profiles.moc.cpp \
	kggz_hosts.moc.cpp \
	kggz_ftp.moc.cpp \
	kggz.moc.cpp \
	kggz_update.moc.cpp \
	kggz_roomsmenu.moc.cpp \
	kggz_browser.moc.cpp \
	kggz_state.moc.cpp \
	kggz_chat.moc.cpp \
	kggz_startup.moc.cpp \
	kggz_tables.moc.cpp \
	kggz_users.moc.cpp \
	kggz_chatwidget.moc.cpp \
	kggz_motd.moc.cpp \
	kggz_teams.moc.cpp \
	kggz_preferences.moc.cpp \
	kggz_games.moc.cpp \
	kggz_settings.moc.cpp \
	kggz_connect.moc.cpp \
	kggz_launch.moc.cpp \
	kggz_server.cpp

noinst_HEADERS = kggz_grubby.h \
	kggz_install.h \
	kggz_playerinfo.h \
	kggz_profiles.h \
	kggz_hosts.h \
	kggz_ftp.h \
	kggz.h \
	kggz_update.h \
	kggz_roomsmenu.h \
	kggz_browser.h \
	kggz_state.h \
	kggz_chat.h \
	kggz_startup.h \
	kggz_tables.h \
	kggz_users.h \
	kggz_chatwidget.h \
	kggz_motd.h \
	kggz_teams.h \
	kggz_preferences.h \
	kggz_games.h \
	kggz_settings.h \
	kggz_connect.h \
	kggz_launch.h \
	kggz_server.cpp

appsdir = $(kde_appsdir)/Games
apps_DATA = images/wm/kggz.desktop

appdir = $(kde_datadir)/kggz
app_DATA = images/startup.png

KDE_ICON = kggz

EXTRA_DIST = $(apps_DATA) $(app_DATA)

#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o kggz.pot

