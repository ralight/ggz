Using GGZ with a CMake build system
===================================

CMake (http://www.cmake.org) is an alternative to the autotools and as such
used by several projects. For those projects ggz.m4 and other autotools
support files from GGZ are of no use. Instead, they want to add GGZ
functionality using the existing CMake facilities.

With GGZ.cmake, this is now possible.
In a project's CMakeLists.txt, this file is included and its contents can
then be used.
The following functionality will be covered by GGZ.cmake:
* check for ggz-config                [done]
* installation of module.dsc          [done]
* inclusion of static kggzmod library [done]
* check for other GGZ libraries       [missing]

The module.dsc installation requires ggz-config and works as follows:
  include(GGZ.cmake)
  register_ggz_module(module.dsc)
Nothing is registered if ggz-config cannot be found.

The kggzmod integration works as follows:
  include_directories(... ${KGGZMOD_INCLUDE_DIR})
  target_link_libraries(foogame ... ${KGGZMOD_LIBS})
It currently needs a 'kggzmod' directory in the project directory.

