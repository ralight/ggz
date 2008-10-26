SET(LOKAREST_FOUND FALSE)

FIND_PATH(LOKAREST_INCLUDES lokarest.h PATHS
  ${KDE4_INCLUDE_DIR}
  ${INCLUDE_INSTALL_DIR}
  NO_CMAKE_SYSTEM_PATH
)

FIND_PATH(LOKAREST_INCLUDES lokarest.h)

FIND_LIBRARY(LOKAREST_LIBRARY NAMES lokarest PATHS
  ${KDE4_LIB_DIR}
  ${LIB_INSTALL_DIR}
  NO_CMAKE_SYSTEM_PATH
)

FIND_LIBRARY(LOKAREST_LIBRARY NAMES lokarest)

if(LOKAREST_LIBRARY AND LOKAREST_INCLUDES)
  set(LOKAREST_FOUND TRUE)
endif(LOKAREST_LIBRARY AND LOKAREST_INCLUDES)

if(LOKAREST_FOUND)
  MESSAGE(STATUS "Found lokarest (library ${LOKAREST_LIBRARY}, headers ${LOKAREST_INCLUDES}).")
else(LOKAREST_FOUND)
  MESSAGE(STATUS "Could not find lokarest. No synchronisation with GGZ Community possible.")
endif(LOKAREST_FOUND)