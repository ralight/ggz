project(vencedor)

add_subdirectory(icons)
add_subdirectory(doc)

set(vencedor_SRCS
	vencedor.cpp
	motd.cpp
	main.cpp
)

include_directories(${CMAKE_BUILD_DIR})

kde4_add_kcfg_files(vencedor_SRCS prefs.kcfgc)

kde4_add_executable(vencedor ${vencedor_SRCS})

#kde4_add_app_icon(vencedor_SRCS hi*-app-vencedor.png)
#kde4_install_icons(${ICON_INSTALL_DIR})

target_link_libraries(vencedor ${KDE4_KDEUI_LIBS} ${KDE4_KHTML_LIBS} -L../lib kggzcore kggzgames kggzlib)

install(TARGETS vencedor ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES vencedor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES ggz.protocol DESTINATION ${SERVICES_INSTALL_DIR})

#install(FILES bg.png DESTINATION ${DATA_INSTALL_DIR}/vencedor)

