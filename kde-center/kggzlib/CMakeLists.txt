project(kggzlib)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

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
	wsinteractor.cpp
)

kde4_add_library(kggzlib SHARED ${kggzlib_LIB_SRCS})

#set_target_properties(kggzlib PROPERTIES COMPILE_FLAGS -fPIC)
target_link_libraries(kggzlib ${KDE4_KIO_LIBS} ${QT_QTNETWORK_LIBRARY} ${QT_QTXML_LIBRARY} kggzcore)
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
	wsinteractor.h
	DESTINATION ${INCLUDE_INSTALL_DIR}/kggzlib
)

install(FILES
	players/admin.png
	players/banned.png
	players/botgrubby.png
	players/bot.png
	players/buddy.png
	players/guest.png
	players/host.png
	players/lag0.png
	players/lag1.png
	players/lag2.png
	players/lag3.png
	players/lag4.png
	players/lag5.png
	players/player.png
	players/team-founder.png
	players/team-member.png
	players/you.png
	DESTINATION ${DATA_INSTALL_DIR}/kggzlib/players
)

install(FILES
	icons/dialog-cancel.png
	icons/rating.png
	icons/tagua.png
	icons/ggzmetaserv.png
	DESTINATION ${DATA_INSTALL_DIR}/kggzlib/icons
)

install(FILES
	games/chess.png
	games/dots.png
	games/tictactoe.png
	DESTINATION ${DATA_INSTALL_DIR}/kggzlib/games
)

install(FILES kggzlib.rc DESTINATION ${CONFIG_INSTALL_DIR})
