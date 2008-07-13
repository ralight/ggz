project(ktictactux)

cmake_minimum_required(VERSION 2.6)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${KDE4_INCLUDES})



set(ktictactux_SRCS
	ktictactux.cpp
	ktictactuxproto.cpp
	ktictactuxwin.cpp
	main.cpp
	qwhiteframe.cpp)

kde4_add_executable(ktictactux ${ktictactux_SRCS})

kde4_add_app_icon(ktictactux_SRCS hi*-app-ktictactux.png)

target_link_libraries(ktictactux ${KDE4_KDEUI_LIBS} ${KDE4_KNEWSTUFF2_LIBS} -L../../kggzcore/ kggzcore.so kggzmod kggzgames)

install(TARGETS ktictactux ${INSTALL_TARGETS_DEFAULT_ARGS})
#install(FILES ktictactux.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES bg.png DESTINATION ${DATA_INSTALL_DIR}/ktictactux)

