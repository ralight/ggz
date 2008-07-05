cmake_minimum_required(VERSION 2.6)

project(plasma-kggz)
 
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(Plasma REQUIRED)
 
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   ..
)
 
set(plasmakggz_SRCS plasma-kggz.cpp configwidget.cpp)
 
kde4_add_plugin(plasma_applet_kggz ${plasmakggz_SRCS})
#target_link_libraries(plasma_applet_kggz
#                      ${PLASMA_LIBS} ${KDE4_KDEUI_LIBS})
target_link_libraries(plasma_applet_kggz
   -lplasma ${KDE4_KDEUI_LIBS}
   -L../kggzcore -lkggzcore)
 
install(TARGETS plasma_applet_kggz
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-kggz.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

kde4_install_icons(${ICON_INSTALL_DIR})
