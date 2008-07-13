project(kggzdmod)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.. ${GGZDMOD_INCLUDES})

set(kggzdmod_LIB_SRCS
	event.cpp
	module.cpp
	player.cpp
	request.cpp
)

kde4_add_library(kggzdmod SHARED ${kggzdmod_LIB_SRCS})

target_link_libraries(kggzdmod ${KDE4_KDECORE_LIBS} ${QT_QTNETWORK_LIBRARY} ${GGZDMOD_LIBRARY})
