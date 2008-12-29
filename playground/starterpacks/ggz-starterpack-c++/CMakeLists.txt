project(ggz-starterpack-c++)

cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

include(GGZ)

set(GGZ_REGISTRYNAME "ggz-starterpack-c++")

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${KDE4_INCLUDES})

add_subdirectory(ggz-sample-client)
add_subdirectory(ggz-sample-server)
