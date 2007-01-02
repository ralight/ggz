# Installation of GGZ game modules and GGZ library checks
# Josef Spillner <josef@ggzgamingzone.org> 2006
# see install_pam_service for some comments on DESTDIR etc.

###########################################################
# Check for existence of 'ggz-config' tool

FIND_PROGRAM(GGZCONFIG_EXECUTABLE NAMES ggz-config)

if(GGZCONFIG_EXECUTABLE)
    set(GGZCONFIG_FOUND 1)
else(GGZCONFIG_EXECUTABLE)
    MESSAGE(STATUS "Warning: ggz-config not found, game modules will not be registered!")
endif(GGZCONFIG_EXECUTABLE)

###########################################################
# Installation hook for module.dsc files

if(GGZCONFIG_FOUND)
    macro(register_ggz_module MODFILE)
        install(CODE "
            exec_program(${GGZCONFIG_EXECUTABLE} ARGS --install --force --modfile=${CMAKE_CURRENT_SOURCE_DIR}/${MODFILE})
        ")
    endmacro(register_ggz_module)
else(GGZCONFIG_FOUND)
    macro(register_ggz_module MODFILE)
    endmacro(register_ggz_module)
endif(GGZCONFIG_FOUND)

###########################################################
# Check for the location of 'kggzmod' library

set(KGGZMOD_LIBS kggzmod)
set(KGGZMOD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/kreversi)
# FIXME: we don't really check yet and always use the internal copy

