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
set_target_properties(kggzdmod PROPERTIES SOVERSION 0.0.0)

install(TARGETS kggzdmod ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
	event.h
	module.h
	player.h
	request.h
	DESTINATION ${INCLUDE_INSTALL_DIR}/kggzdmod
)
