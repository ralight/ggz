project(ggz-kde-center)

cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

include(GGZKDECenterPackage)
include(GGZ)

set(GGZ_REGISTRYNAME "ggz-kde-center")

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${KDE4_INCLUDES})

find_package(GGZDMod REQUIRED)
find_package(GGZCore REQUIRED)

add_subdirectory(kggzdmod)
add_subdirectory(kggzcore)
add_subdirectory(kggzlib)
add_subdirectory(plasma-kggz)
add_subdirectory(kgrubby)
add_subdirectory(ktictactux)
add_subdirectory(kryds)
add_subdirectory(kdots)
