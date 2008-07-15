project(kdots)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Dot)

set(kdots_SRCS
	main.cpp
	kdots.cpp
	kdots_about.cpp
	kdots_options.cpp
	kdots_help.cpp
	kdots_proto.cpp
	kdots_replay.cpp
	kdots_win.cpp
)

kde4_add_executable(kdots ${kdots_SRCS})

kde4_add_app_icon(kdots_SRCS hi*-app-kdots.png)
kde4_install_icons(${ICON_INSTALL_DIR})

target_link_libraries(kdots ${KDE4_KDEUI_LIBS} ${KDE4_KNEWSTUFF2_LIBS} kggzmod kggzgames dot ${GGZDMOD_LIBRARY} ggzmod ggz)

install(TARGETS kdots ${INSTALL_TARGETS_DEFAULT_ARGS})
#install(FILES kdots.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
#install(FILES kdots.knsrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES
	dragon.png
	snowdragon.png
	firedragon.png
	DESTINATION ${DATA_INSTALL_DIR}/kdots
)

add_subdirectory(Dot)
