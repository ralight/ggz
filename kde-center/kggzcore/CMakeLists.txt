project(kggzcore)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.. ${GGZCORE_INCLUDES})

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

kde4_add_library(kggzcore SHARED ${kggzcore_LIB_SRCS})

target_link_libraries(kggzcore ${KDE4_KDECORE_LIBS} ${QT_QTNETWORK_LIBRARY} ${GGZCORE_LIBRARY})
set_target_properties(kggzcore PROPERTIES SOVERSION 0.0.0)

install(TARGETS kggzcore ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
	coreclient.h
	error.h
	misc.h
	room.h
	player.h
	table.h
	DESTINATION ${INCLUDE_INSTALL_DIR}/kggzcore
)
