project(ktictactux)

set(ktictactux_SRCS
	ktictactux.cpp
	ktictactuxproto.cpp
	ktictactuxwin.cpp
	main.cpp
	qwhiteframe.cpp)

kde4_add_executable(ktictactux ${ktictactux_SRCS})

kde4_add_app_icon(ktictactux_SRCS hi*-app-ktictactux.png)
kde4_install_icons(${ICON_INSTALL_DIR})

target_link_libraries(ktictactux ${KDE4_KDEUI_LIBS} ${KDE4_KNEWSTUFF2_LIBS} -L../lib -lkggzcore kggzmod kggzgames)

install(TARGETS ktictactux ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ktictactux.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES bg.png DESTINATION ${DATA_INSTALL_DIR}/ktictactux)

add_subdirectory(themes)
