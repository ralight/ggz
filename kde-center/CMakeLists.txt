project(ggz-kde-center)

cmake_minimum_required(VERSION 2.6)

# FIXME: detect dynamically!
set(GGZ_INCLUDES "/home/josef/sys/include")

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${KDE4_INCLUDES} ${GGZ_INCLUDES})

add_subdirectory(kggzdmod)
add_subdirectory(kggzcore)
add_subdirectory(plasma-kggz)
add_subdirectory(kgrubby)
add_subdirectory(ktictactux)
add_subdirectory(kryds)
