# AC_GGZ_DEBUG
# $Id: ggz-debug.m4 5763 2004-01-30 10:03:03Z josef $
#
# Check for debugging choices.  Sets $enable_debug, $enable_debug_gdb,
# and $enable_debug_mem.
#
# This file may be used in many GGZ modules; the master copy should reside
# in libggz.

AC_DEFUN([AC_GGZ_DEBUG],
[
  # This defines either DEBUG or NDEBUG.  We could provide a third option
  # where neither are defined.
  AC_ARG_ENABLE([debug],
		AC_HELP_STRING([--disable-debug], [disable basic debugging]),
		[enable_debug=no],
		[enable_debug=yes])

  if test "$enable_debug" = yes; then
    AC_DEFINE([DEBUG], 1, [Define to enable basic debugging])
    DFLAGS="DEBUG"
  else
    AC_DEFINE([NDEBUG], 1, [Define to disable basic debugging])
    DFLAGS="NDEBUG"
  fi

  # Turn on GDB debugging, via a command-line argument for gcc
  AC_ARG_ENABLE([debug-gdb],
		AC_HELP_STRING([--enable-debug-gdb], [enable GDB debugging]),
		[enable_debug_gdb=yes],
		[enable_debug_gdb=no])
  if test "$enable_debug_gdb" = yes; then
    CFLAGS="$CFLAGS -ggdb"
  fi

  # Turn on malloc debugging via dmalloc (if specified)
  AC_ARG_ENABLE([debug-mem],
		AC_HELP_STRING([--enable-debug-mem],[enable mem debugging]),
		[enable_debug_mem=yes],
		[enable_debug_mem=no])
  if test "$enable_debug_mem" = yes; then
    AC_DEFINE([DEBUG_MEM], 1, [Define to use dmalloc memory debugging])

    # We count on this code coming after DEBUG_MEM is defined...
    AH_VERBATIM([_DEBUG_MEM],
[/* Use dmalloc for memory debugging */
#ifdef DEBUG_MEM
# include <dmalloc.h>
#endif])
    DFLAGS="$DFLAGS DEBUG_MEM"
    LDADD="$LDADD -ldmalloc"
  fi

  # Use -Wall if we have gcc.
  # FIXME: Rewrite this to be comprehensible.
  changequote(,)dnl
  if test "x$GCC" = "xyes"; then
    case " $CFLAGS " in
    *[\ \	]-Wall[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -Wall" ;;
    esac
  fi
  changequote([,])dnl
])
