dnl Configuration for GGZ-SDL-Games
dnl ===============================

AC_INIT([GGZ SDL games], [0.0.11], [ggz-dev@mail.ggzgamingzone.org], [ggz-sdl-games])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Basic checks
dnl ============

AC_PROG_CC
AC_PROG_CPP
AC_ISC_POSIX
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl Check for header files
dnl ======================
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([winsock.h fnmatch.h])

dnl Compiler stuff
dnl ==============

CFLAGS="$CFLAGS -Wall"

dnl Check for SDL and OpenGL
dnl ========================

# blah blah... found it :)
AM_PATH_SDL(1.2.0)

AC_CHECK_LIB(SDL_image, IMG_Load,
    [AC_CHECK_HEADER(SDL/SDL_image.h, ,
        AC_MSG_ERROR([*** Cannot find SDL/SDL_image.h header]))],
    AC_MSG_ERROR([*** Cannot find SDL_image library]))

AC_CHECK_LIB(SDL_ttf, TTF_Init,
    [AC_CHECK_HEADER(SDL/SDL_ttf.h, ,
        AC_MSG_ERROR([*** Cannot find SDL/SDL_ttf.h header]))],
    AC_MSG_ERROR([*** Cannot find SDL_ttf library]))

sdlmixer=no
AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio,
    [AC_CHECK_HEADER(SDL/SDL_mixer.h, [sdlmixer=yes],
        [AC_MSG_WARN([*** Cannot find SDL/SDL_mixer.h header])])],
    [AC_MSG_WARN([*** Cannot find SDL_mixer library])])
if test "x$sdlmixer" = "xyes"; then
    LIB_SOUND="-lSDL_mixer"
    AC_SUBST(LIB_SOUND)
    AC_DEFINE_UNQUOTED(HAVE_SOUND, 1, [Sound support])
fi

AC_CHECK_LIB(GL, glRotatef,
    [AC_CHECK_HEADER(GL/gl.h, ,
        AC_MSG_ERROR([*** Cannot find GL/gl.h header]))],
    AC_MSG_ERROR([*** Cannot find GL library]))

xcursor=no
AC_CHECK_LIB(Xcursor, XcursorLibraryLoadCursor,
    [AC_CHECK_HEADER(X11/Xcursor/Xcursor.h, [xcursor=yes],
        [AC_MSG_WARN(*** Cannot find X11/Xcursor/Xcursor.h header)],
        [#include <X11/Xlib.h>])],
    [AC_MSG_WARN(*** Cannot find Xcursor library )],
    [-L /usr/X11R6/lib -l Xrender])
if test "x$xcursor" = "xyes"; then
    XCURSOR_LDFLAGS="-L /usr/X11R6/lib"
    LIB_XCURSOR="-lXcursor -lXrender"
    AC_SUBST(XCURSOR_LDFLAGS)
    AC_SUBST(LIB_XCURSOR)
    AC_DEFINE_UNQUOTED(HAVE_XCURSOR, 1, [Xcursor support])
fi

dnl Check for GGZ requirements
dnl ==========================

AC_GGZ_INIT(defaults, export)
AC_GGZ_INTL
AC_GGZ_LIBGGZ
AC_GGZ_GGZMOD
AC_GGZ_CONFIG("true", ignore)

dnl Create Makefiles
dnl ================

AC_CONFIG_FILES([Makefile
	geekgame/Makefile
	geekgame/module.dsc
	geekgame/graphics/Makefile
	ttt3d/Makefile
	ttt3d/module.dsc])
AC_OUTPUT

dnl Report success
dnl ==============

echo ""
echo "GGZ-SDL-Games is now configured. Please type 'make' in order to compile it."
echo "The following installation layout is used:"
echo ""
echo "Generic installation prefix: $prefix (for libraries etc.)"
echo "Game executable directory: $ggzexecmoddir"
echo "Game data directory: $ggzdatadir"
echo ""
echo "Compilation support:"
echo "Sound (via SDL_mixer)..................$sdlmixer"
echo "Coloured mouse cursors (via XCursor)...$xcursor"
echo ""
if test "x$GGZ_CONFIG" = "x/bin/true"; then
	echo "Warning: game information files will not be installed."
	echo "This should only happen during binary package creation."
	echo "Packages must install the .dsc files upon installation."
	echo ""
fi
