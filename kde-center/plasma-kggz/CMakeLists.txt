project(plasma-kggz)

# KDE < 4.3 only
#find_package(Plasma REQUIRED)

include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   ..
)

set(plasmakggz_SRCS
	plasma-kggz.cpp
)

kde4_add_plugin(plasma_applet_kggz ${plasmakggz_SRCS})
#target_link_libraries(plasma_applet_kggz
#                      ${PLASMA_LIBS} ${KDE4_KDEUI_LIBS})
target_link_libraries(plasma_applet_kggz
   -lplasma ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS}
   -L../lib kggzcore kggzlib)

install(TARGETS plasma_applet_kggz
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasma-applet-kggz.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

kde4_install_icons(${ICON_INSTALL_DIR})
