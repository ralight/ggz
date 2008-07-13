project(kggzcore)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(kggzcore_LIB_SRCS
	coreclient.cpp
	coreclientbase.cpp
	error.cpp
	misc.cpp
	room.cpp
	roombase.cpp
	table.cpp
	player.cpp
)

#kde4_automoc(${kggzcore_LIB_SRCS})

kde4_add_library(kggzcore SHARED ${kggzcore_LIB_SRCS})

target_link_libraries(kggzcore ${KDE4_KDECORE_LIBS} ${QT_QTNETWORK_LIBRARY} ggzcore)
