# Installation of GGZ game modules and GGZ library checks
# Josef Spillner <josef@ggzgamingzone.org> 2006 - 2008
# see install_pam_service for some comments on DESTDIR etc.

###########################################################
# Documentation
# - call register_ggz_module(module.dsc) in your game client's CMakeLists.txt
# - use GGZ_{GAME/ROOM}DESC_INSTALL_DIR in your game server's CMakeLists.txt
# - set -DGGZ_NOREGISTRY=/usr/share/ggz/modules/kdegames when building binary packages
#   (mostly obsolete now)
# - set -DGGZ_FORCEREGISTRY=1 to force registration in out-of-prefix installations
# - set GGZ_REGISTRYNAME in top-level CMakeLists.txt to package name

###########################################################
# Check for existence of 'ggz-config' tool

FIND_PROGRAM(GGZCONFIG_EXECUTABLE NAMES ggz-config)

if(GGZCONFIG_EXECUTABLE)
    set(GGZCONFIG_FOUND 1)
    MESSAGE(STATUS "GGZ: Success: ggz-config was found as ${GGZCONFIG_EXECUTABLE}.")
else(GGZCONFIG_EXECUTABLE)
    MESSAGE(STATUS "GGZ: Warning: ggz-config not found, game modules will not be registered!")
    MESSAGE(STATUS " Source package containing ggz-config: 'ggz-base-libs'")
    MESSAGE(STATUS " Binary package containing ggz-config: 'ggzcore-bin' (or similar)")
endif(GGZCONFIG_EXECUTABLE)

###########################################################
# Installation hook for module.dsc files
# Documentation is in GGZ SVN under docs/ggz-project/packagers

# FIXME: improve cmake to allow typed options
#        option(GGZ_NOREGISTRY PATH "Directory where to install
#        the module.dsc files to")
# FIXME: Bug in cmake? REG needs to be declared whenever
#        -DGGZ_NOREGISTRY is given. However, why is that, especially since
#        the error occurs in the 'else' tree which isn't used in that case.
# FIXME: should we promote KDE4_PREFIX to FindKDE4Internal.cmake?

exec_program(${KDE4_KDECONFIG_EXECUTABLE} ARGS "--prefix" OUTPUT_VARIABLE KDE4_PREFIX)
if(NOT KDE4_PREFIX STREQUAL "${CMAKE_INSTALL_PREFIX}")
    MESSAGE(STATUS "GGZ: Warning: Out-of-KDE-prefix installation, make sure to include ${KDE4_PREFIX} in $KDEDIRS at runtime.")
    MESSAGE(STATUS " KDE Games prefix: ${KDE4_PREFIX}")
    MESSAGE(STATUS " Prefix for this package: ${CMAKE_INSTALL_PREFIX}")
endif(NOT KDE4_PREFIX STREQUAL "${CMAKE_INSTALL_PREFIX}")

if(GGZCONFIG_FOUND)
    if(GGZ_NOREGISTRY)
        set(REG "" STRING)
        set(NOREG "--noregistry=${GGZ_NOREGISTRY}")
        MESSAGE(STATUS "GGZ: Installing game module information to ${GGZ_NOREGISTRY}.")
        MESSAGE(STATUS "GGZ: Post-registration hooks need to be called by distributors.")
    else(GGZ_NOREGISTRY)
        EXECUTE_PROCESS(COMMAND ${GGZCONFIG_EXECUTABLE} --configdir OUTPUT_VARIABLE REG)
        STRING(REPLACE "\n" "" REG "${REG}")
        if(SYSCONF_INSTALL_DIR STREQUAL "${REG}")
            MESSAGE(STATUS "GGZ: Registering game module information into ${REG}.")
        else(SYSCONF_INSTALL_DIR STREQUAL "${REG}")
            if(GGZ_FORCEREGISTRY)
                MESSAGE(STATUS "GGZ: Forced registration into ${REG}.")
            else(GGZ_FORCEREGISTRY)
                set(SKIP_GGZCONFIG TRUE BOOL)
                MESSAGE(STATUS "GGZ: Warning: Out-of-GGZ-prefix installation, no registration unless -DGGZ_FORCEREGISTRY=1 is given.")
                MESSAGE(STATUS " KDE Games configuration gets installed into: ${SYSCONF_INSTALL_DIR}")
                MESSAGE(STATUS " GGZ configuration is expected in: ${REG}")
            endif(GGZ_FORCEREGISTRY)
        endif(SYSCONF_INSTALL_DIR STREQUAL "${REG}")
    endif(GGZ_NOREGISTRY)
else(GGZCONFIG_FOUND)
    set(SKIP_GGZCONFIG TRUE BOOL)
endif(GGZCONFIG_FOUND)

if(NOT EXISTS "${SYSCONF_INSTALL_DIR}/ggzd/ggzd.motd")
    MESSAGE(STATUS "GGZ: Warning: No GGZ server found in ${SYSCONF_INSTALL_DIR}/ggzd, game server files may not be found.")
endif(NOT EXISTS "${SYSCONF_INSTALL_DIR}/ggzd/ggzd.motd")
set(GGZ_GAMEDESC_INSTALL_DIR "${SYSCONF_INSTALL_DIR}/ggzd/games")
set(GGZ_ROOMDESC_INSTALL_DIR "${SYSCONF_INSTALL_DIR}/ggzd/rooms")

if(SKIP_GGZCONFIG)
    macro(register_ggz_module MODFILE)
    endmacro(register_ggz_module)
    MESSAGE(STATUS "GGZ: Warning: KDE games will not be found by GGZ clients.")
else(SKIP_GGZCONFIG)
    macro(register_ggz_module MODFILE)
        if(GGZ_REGISTRYNAME)
            set(REGARG "--registry=${GGZ_REGISTRYNAME}")
        else(GGZ_REGISTRYNAME)
            set(REGARG "")
        endif(GGZ_REGISTRYNAME)
        install(CODE "
            exec_program(${GGZCONFIG_EXECUTABLE} ARGS --install -D --force ${NOREG} ${REGARG} --modfile=${CMAKE_CURRENT_SOURCE_DIR}/${MODFILE})
        ")
    endmacro(register_ggz_module)
endif(SKIP_GGZCONFIG)

###########################################################
# Specify the location and dependencies of the GGZ libraries within kdegames

set(KGGZGAMES_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libkdegames)
set(KGGZGAMES_LIBS kggzgames kggzmod kggznet)
set(KGGZMOD_LIBS kggzmod kggznet)
set(KGGZNET_LIBS kggznet)

