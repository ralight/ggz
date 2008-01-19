project(kggzcore)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(kggzcore_LIB_SRCS
    coreclient.cpp
    error.cpp)

kde4_automoc(${kggzcore_LIB_SRCS})

kde4_add_library(kggzcore STATIC ${kggzcore_LIB_SRCS})

target_link_libraries(kggzcore ${QT_QTNETWORK_LIBRARY})

