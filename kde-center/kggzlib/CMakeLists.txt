project(kggzlib)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(kggzlib_LIB_SRCS
	connectiondialog.cpp
	player.cpp
	playerlist.cpp
	qasyncpixmap.cpp
	qfilecache.cpp
	qrecursivesortfilterproxymodel.cpp
)

kde4_add_library(kggzlib STATIC ${kggzlib_LIB_SRCS})

#target_link_libraries(kggzlib ${KDE4_KDECORE_LIBS} ${QT_QTNETWORK_LIBRARY})

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
