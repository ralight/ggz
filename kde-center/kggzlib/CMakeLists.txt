project(kggzlib)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)
if(LOKAREST_FOUND)
  include_directories(${LOKAREST_INCLUDES})
endif(LOKAREST_FOUND)

set(kggzlib_LIB_SRCS
	connectiondialog.cpp
	ggzserver.cpp
	player.cpp
	playerlist.cpp
	qasyncpixmap.cpp
	qfilecache.cpp
	qrecursivesortfilterproxymodel.cpp
	room.cpp
	roomlist.cpp
	roomdelegate.cpp
	roomtreeview.cpp
	serverlist.cpp
	connectionprofiles.cpp
	configwidget.cpp
	roomselector.cpp
	serverselector.cpp
	util.cpp
	ggzprofile.cpp
	kggzcorelayer.cpp
	tableconfiguration.cpp
	kggzaction.cpp
	tablelist.cpp
	tabledialog.cpp
	moduledialog.cpp
	modulelist.cpp
	moduleselector.cpp
)

kde4_add_library(kggzlib SHARED ${kggzlib_LIB_SRCS})

#set_target_properties(kggzlib PROPERTIES COMPILE_FLAGS -fPIC)
target_link_libraries(kggzlib ${KDE4_KIO_LIBS} ${KDE4_KDNSSD_LIBS} ${QT_QTNETWORK_LIBRARY} ${QT_QTXML_LIBRARY} kggzcore)
if(LOKAREST_FOUND)
  target_link_libraries(kggzlib ${LOKAREST_LIBRARY})
endif(LOKAREST_FOUND)

set_target_properties(kggzlib PROPERTIES SOVERSION 0.0.0)

install(TARGETS kggzlib ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
	configwidget.h
	connectiondialog.h
	connectionprofiles.h
	ggzprofile.h
	ggzserver.h
	kggzcorelayer.h
	kggzlib_export.h
	modelview.h
	player.h
	playerlist.h
	qasyncpixmap.h
	qfilecache.h
	qrecursivesortfilterproxymodel.h
	roomdelegate.h
	room.h
	roomlist.h
	roomselector.h
	roomtreeview.h
	serverlist.h
	serverselector.h
	util.h
	ggzprofile.h
	kggzcorelayer.h
	kggzlib_export.h
	tableconfiguration.h
	kggzaction.h
	tablelist.h
	tabledialog.h
	moduledialog.h
	modulelist.h
	moduleselector.h
	DESTINATION ${INCLUDE_INSTALL_DIR}/kggzlib
)

install(FILES
	players/ox-admin.png
	players/ox-admin.svg
	players/ox-banned.png
	players/ox-banned.svg
	players/ox-bot.png
	players/ox-bot.svg
	players/ox-buddy.png
	players/ox-buddy.svg
	players/ox-guest.png
	players/ox-guest.svg
	players/ox-host.png
	players/ox-host.svg
	players/ox-empty.png
	players/ox-empty.svg
	players/ox-player.png
	players/ox-player.svg
	players/ox-self.png
	players/ox-self.svg
	players/botgrubby.png
	players/lag0.png
	players/lag1.png
	players/lag2.png
	players/lag3.png
	players/lag4.png
	players/lag5.png
	players/team-founder.png
	players/team-member.png
	DESTINATION ${DATA_INSTALL_DIR}/kggzlib/players
)

install(FILES
	icons/dialog-cancel.png
	icons/rating.png
	icons/tagua.png
	icons/ggz.png
	icons/ggzmetaserv.png
	icons/server_local.png
	icons/server_local.svg
	icons/server_online.png
	icons/server_online.svg
	DESTINATION ${DATA_INSTALL_DIR}/kggzlib/icons
)

install(FILES
	games/chess.png
	games/dots.png
	games/tictactoe.png
	DESTINATION ${DATA_INSTALL_DIR}/kggzlib/games
)

install(FILES kggzlib.rc DESTINATION ${CONFIG_INSTALL_DIR})
