# $Id: mkdir.m4 5859 2004-02-09 01:38:22Z jdorje $
# FUNC_MKDIR_TAKES_ONE_ARG defines MKDIR_TAKES_ONE_ARG if, well, mkdir takes
# one arg (instead of 2 like it does on POSIX systems).
#
# Take from a phantom file contributed to GNU "patch" that I can't find
# anywhere except in mailing lists.  Attributed to Mumit Khan and Paul Eggert.
#
# Note that if you don't have the correct #includes in the test-compile code,
# the compiler will give a missing-prototype warning but will succeed.  Yuck!

AC_DEFUN([FUNC_MKDIR_TAKES_ONE_ARG],
  [AC_CHECK_FUNCS([mkdir])
   AC_CACHE_CHECK([whether mkdir takes only one argument],
     cv_mkdir_takes_one_arg,
     [cv_mkdir_takes_one_arg=no
      if test $ac_cv_func_mkdir = yes; then
 AC_TRY_COMPILE([
#include <dir.h>
   ],
   [mkdir (".");],
   cv_mkdir_takes_one_arg=yes,
   cv_mkdir_takes_one_arg=no
   )
      fi
     ]
   )
   if test $cv_mkdir_takes_one_arg = yes; then
     AC_DEFINE([MKDIR_TAKES_ONE_ARG], 1,
       [Define if mkdir takes only one argument.])
   fi
  ]
)
