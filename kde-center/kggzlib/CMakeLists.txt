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

