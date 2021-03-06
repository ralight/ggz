project(ggz-kde-center)

cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${KDE4_INCLUDES})

include(GGZKDECenterPackage)
include(GGZ)

set(GGZ_REGISTRYNAME "ggz-kde-center")

macro_optional_find_package(GGZDMod)
macro_log_feature(GGZDMOD_FOUND "ggzdmod" "GGZ base library for game servers" "http://www.ggzgamingzone.org" FALSE "" "Needed to compile the game servers.")

macro_optional_find_package(Lokarest)
macro_log_feature(LOKAREST_FOUND "lokarest" "RESTful web service handling" "http://techbase.kde.org/Projects/LokaRest" FALSE "" "Needed for GGZ Community synchronisation.")

find_package(GGZCore REQUIRED)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if(GGZDMOD_FOUND)
  add_subdirectory(kggzdmod)
endif(GGZDMOD_FOUND)
add_subdirectory(kggzcore)
add_subdirectory(kggzlib)
add_subdirectory(plasma-kggz)
add_subdirectory(kgrubby)
add_subdirectory(ktictactux)
if(GGZDMOD_FOUND)
  add_subdirectory(kryds)
endif(GGZDMOD_FOUND)
add_subdirectory(kdots)
add_subdirectory(vencedor)

macro_display_feature_log()
