# Macro to add for using GNU gettext.
# Ulrich Drepper <drepper@cygnus.com>, 1995.
#
# This file can be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU General Public
# License or the GNU Library General Public License but which still want
# to provide support for the GNU gettext functionality.
# Please note that the actual code of the GNU gettext library is covered
# by the GNU Library General Public License, and the rest of the GNU
# gettext package package is covered by the GNU General Public License.
# They are *not* in the public domain.

# serial 10

dnl Usage: AM_WITH_NLS([TOOLSYMBOL], [NEEDSYMBOL], [LIBDIR]).
dnl If TOOLSYMBOL is specified and is 'use-libtool', then a libtool library
dnl    $(top_builddir)/intl/libintl.la will be created (shared and/or static,
dnl    depending on --{enable,disable}-{shared,static} and on the presence of
dnl    AM-DISABLE-SHARED). Otherwise, a static library
dnl    $(top_builddir)/intl/libintl.a will be created.
dnl If NEEDSYMBOL is specified and is 'need-ngettext', then GNU gettext
dnl    implementations (in libc or libintl) without the ngettext() function
dnl    will be ignored.
dnl LIBDIR is used to find the intl libraries.  If empty,
dnl    the value `$(top_builddir)/intl/' is used.
dnl
dnl The result of the configuration is one of three cases:
dnl 1) GNU gettext, as included in the intl subdirectory, will be compiled
dnl    and used.
dnl    Catalog format: GNU --> install in $(datadir)
dnl    Catalog extension: .mo after installation, .gmo in source tree
dnl 2) GNU gettext has been found in the system's C library.
dnl    Catalog format: GNU --> install in $(datadir)
dnl    Catalog extension: .mo after installation, .gmo in source tree
dnl 3) No internationalization, always use English msgid.
dnl    Catalog format: none
dnl    Catalog extension: none
dnl The use of .gmo is historical (it was needed to avoid overwriting the
dnl GNU format catalogs when building on a platform with an X/Open gettext),
dnl but we keep it in order not to force irrelevant filename changes on the
dnl maintainers.
dnl
AC_DEFUN([AM_WITH_NLS],
  [AC_MSG_CHECKING([whether NLS is requested])
    dnl Default is enabled NLS
    AC_ARG_ENABLE(nls,
      [  --disable-nls           do not use Native Language Support],
      USE_NLS=$enableval, USE_NLS=yes)
    AC_MSG_RESULT($USE_NLS)
    AC_SUBST(USE_NLS)

    BUILD_INCLUDED_LIBINTL=no
    USE_INCLUDED_LIBINTL=no
    INTLLIBS=

    dnl If we use NLS figure out what method
    if test "$USE_NLS" = "yes"; then
      AC_DEFINE(ENABLE_NLS, 1,
        [Define to 1 if translation of program messages to the user's native language
   is requested.])
      AC_MSG_CHECKING([whether included gettext is requested])
      AC_ARG_WITH(included-gettext,
        [  --with-included-gettext use the GNU gettext library included here],
        nls_cv_force_use_gnu_gettext=$withval,
        nls_cv_force_use_gnu_gettext=no)
      AC_MSG_RESULT($nls_cv_force_use_gnu_gettext)

      nls_cv_use_gnu_gettext="$nls_cv_force_use_gnu_gettext"
      if test "$nls_cv_force_use_gnu_gettext" != "yes"; then
        dnl User does not insist on using GNU NLS library.  Figure out what
        dnl to use.  If GNU gettext is available we use this.  Else we have
        dnl to fall back to GNU NLS library.
	CATOBJEXT=NONE

        dnl Add a version number to the cache macros.
        define(gt_cv_func_gnugettext_libc, [gt_cv_func_gnugettext]ifelse([$2], need-ngettext, 2, 1)[_libc])
        define(gt_cv_func_gnugettext_libintl, [gt_cv_func_gnugettext]ifelse([$2], need-ngettext, 2, 1)[_libintl])

	AC_CHECK_HEADER(libintl.h,
	  [AC_CACHE_CHECK([for GNU gettext in libc], gt_cv_func_gnugettext_libc,
	    [AC_TRY_LINK([#include <libintl.h>
extern int _nl_msg_cat_cntr;],
	       [bindtextdomain ("", "");
return (int) gettext ("")]ifelse([$2], need-ngettext, [ + (int) ngettext ("", "", 0)], [])[ + _nl_msg_cat_cntr],
	       gt_cv_func_gnugettext_libc=yes,
	       gt_cv_func_gnugettext_libc=no)])

	   if test "$gt_cv_func_gnugettext_libc" != "yes"; then
	     AC_CACHE_CHECK([for GNU gettext in libintl],
	       gt_cv_func_gnugettext_libintl,
	       [gt_save_LIBS="$LIBS"
		LIBS="$LIBS -lintl $LIBICONV"
		AC_TRY_LINK([#include <libintl.h>
extern int _nl_msg_cat_cntr;],
		  [bindtextdomain ("", "");
return (int) gettext ("")]ifelse([$2], need-ngettext, [ + (int) ngettext ("", "", 0)], [])[ + _nl_msg_cat_cntr],
		  gt_cv_func_gnugettext_libintl=yes,
		  gt_cv_func_gnugettext_libintl=no)
		LIBS="$gt_save_LIBS"])
	   fi

	   dnl If an already present or preinstalled GNU gettext() is found,
	   dnl use it.  But if this macro is used in GNU gettext, and GNU
	   dnl gettext is already preinstalled in libintl, we update this
	   dnl libintl.  (Cf. the install rule in intl/Makefile.in.)
	   if test "$gt_cv_func_gnugettext_libc" = "yes" \
	      || { test "$gt_cv_func_gnugettext_libintl" = "yes" \
		   && test "$PACKAGE" != gettext; }; then
	     AC_DEFINE(HAVE_GETTEXT, 1,
               [Define if the GNU gettext() function is already present or preinstalled.])

	     if test "$gt_cv_func_gnugettext_libintl" = "yes"; then
	       dnl If iconv() is in a separate libiconv library, then anyone
	       dnl linking with libintl{.a,.so} also needs to link with
	       dnl libiconv.
	       INTLLIBS="-lintl $LIBICONV"
	     fi

	     gt_save_LIBS="$LIBS"
	     LIBS="$LIBS $INTLLIBS"
	     AC_CHECK_FUNCS(dcgettext)
	     LIBS="$gt_save_LIBS"

	     dnl Search for GNU msgfmt in the PATH.
	     AM_PATH_PROG_WITH_TEST(MSGFMT, msgfmt,
	       [$ac_dir/$ac_word --statistics /dev/null >/dev/null 2>&1], :)
	     AC_PATH_PROG(GMSGFMT, gmsgfmt, $MSGFMT)

	     dnl Search for GNU xgettext in the PATH.
	     AM_PATH_PROG_WITH_TEST(XGETTEXT, xgettext,
	       [$ac_dir/$ac_word --omit-header /dev/null >/dev/null 2>&1], :)

	     CATOBJEXT=.gmo
	   fi
	])

        if test "$CATOBJEXT" = "NONE"; then
	  dnl GNU gettext is not found in the C library.
	  dnl Fall back on GNU gettext library.
	  nls_cv_use_gnu_gettext=yes
        fi
      fi

      if test "$nls_cv_use_gnu_gettext" = "yes"; then
        dnl Mark actions used to generate GNU NLS library.
        INTLOBJS="\$(GETTOBJS)"
        AM_PATH_PROG_WITH_TEST(MSGFMT, msgfmt,
	  [$ac_dir/$ac_word --statistics /dev/null >/dev/null 2>&1], :)
        AC_PATH_PROG(GMSGFMT, gmsgfmt, $MSGFMT)
        AM_PATH_PROG_WITH_TEST(XGETTEXT, xgettext,
	  [$ac_dir/$ac_word --omit-header /dev/null >/dev/null 2>&1], :)
        AC_SUBST(MSGFMT)
	BUILD_INCLUDED_LIBINTL=yes
	USE_INCLUDED_LIBINTL=yes
        CATOBJEXT=.gmo
	INTLLIBS="ifelse([$3],[],\$(top_builddir)/intl,[$3])/libintl.ifelse([$1], use-libtool, [l], [])a $LIBICONV"
	LIBS=`echo " $LIBS " | sed -e 's/ -lintl / /' -e 's/^ //' -e 's/ $//'`
      fi

      dnl This could go away some day; the PATH_PROG_WITH_TEST already does it.
      dnl Test whether we really found GNU msgfmt.
      if test "$GMSGFMT" != ":"; then
	dnl If it is no GNU msgfmt we define it as : so that the
	dnl Makefiles still can work.
	if $GMSGFMT --statistics /dev/null >/dev/null 2>&1; then
	  : ;
	else
	  AC_MSG_RESULT(
	    [found msgfmt program is not GNU msgfmt; ignore it])
	  GMSGFMT=":"
	fi
      fi

      dnl This could go away some day; the PATH_PROG_WITH_TEST already does it.
      dnl Test whether we really found GNU xgettext.
      if test "$XGETTEXT" != ":"; then
	dnl If it is no GNU xgettext we define it as : so that the
	dnl Makefiles still can work.
	if $XGETTEXT --omit-header /dev/null >/dev/null 2>&1; then
	  : ;
	else
	  AC_MSG_RESULT(
	    [found xgettext program is not GNU xgettext; ignore it])
	  XGETTEXT=":"
	fi
      fi

      dnl We need to process the po/ directory.
      POSUB=po
    fi
    AC_OUTPUT_COMMANDS(
     [for ac_file in $CONFIG_FILES; do
        # Support "outfile[:infile[:infile...]]"
        case "$ac_file" in
          *:*) ac_file=`echo "$ac_file"|sed 's%:.*%%'` ;;
        esac
        # PO directories have a Makefile.in generated from Makefile.in.in.
        case "$ac_file" in */Makefile.in)
          # Adjust a relative srcdir.
          ac_dir=`echo "$ac_file"|sed 's%/[^/][^/]*$%%'`
          ac_dir_suffix="/`echo "$ac_dir"|sed 's%^\./%%'`"
          ac_dots=`echo "$ac_dir_suffix"|sed 's%/[^/]*%../%g'`
          # In autoconf-2.13 it is called $ac_given_srcdir.
          # In autoconf-2.50 it is called $srcdir.
          test -n "$ac_given_srcdir" || ac_given_srcdir="$srcdir"
          case "$ac_given_srcdir" in
            .)  top_srcdir=`echo $ac_dots|sed 's%/$%%'` ;;
            /*) top_srcdir="$ac_given_srcdir" ;;
            *)  top_srcdir="$ac_dots$ac_given_srcdir" ;;
          esac
          if test -f "$ac_given_srcdir/$ac_dir/POTFILES.in"; then
            rm -f "$ac_dir/POTFILES"
            test -n "$as_me" && echo "$as_me: creating $ac_dir/POTFILES" || echo "creating $ac_dir/POTFILES"
            sed -e "/^#/d" -e "/^[ 	]*\$/d" -e "s,.*,     $top_srcdir/& \\\\," -e "\$s/\(.*\) \\\\/\1/" < "$ac_given_srcdir/$ac_dir/POTFILES.in" > "$ac_dir/POTFILES"
            test -n "$as_me" && echo "$as_me: creating $ac_dir/Makefile" || echo "creating $ac_dir/Makefile"
            sed -e "/POTFILES =/r $ac_dir/POTFILES" "$ac_dir/Makefile.in" > "$ac_dir/Makefile"
          fi
          ;;
        esac
      done])


    dnl If this is used in GNU gettext we have to set BUILD_INCLUDED_LIBINTL
    dnl to 'yes' because some of the testsuite requires it.
    if test "$PACKAGE" = gettext; then
      BUILD_INCLUDED_LIBINTL=yes
    fi

    dnl intl/plural.c is generated from intl/plural.y. It requires bison,
    dnl because plural.y uses bison specific features. It requires at least
    dnl bison-1.26 because earlier versions generate a plural.c that doesn't
    dnl compile.
    dnl bison is only needed for the maintainer (who touches plural.y). But in
    dnl order to avoid separate Makefiles or --enable-maintainer-mode, we put
    dnl the rule in general Makefile. Now, some people carelessly touch the
    dnl files or have a broken "make" program, hence the plural.c rule will
    dnl sometimes fire. To avoid an error, defines BISON to ":" if it is not
    dnl present or too old.
    AC_CHECK_PROGS([INTLBISON], [bison])
    if test -z "$INTLBISON"; then
      ac_verc_fail=yes
    else
      dnl Found it, now check the version.
      AC_MSG_CHECKING([version of bison])
changequote(<<,>>)dnl
      ac_prog_version=`$INTLBISON --version 2>&1 | sed -n 's/^.*GNU Bison.* \([0-9]*\.[0-9.]*\).*$/\1/p'`
      case $ac_prog_version in
        '') ac_prog_version="v. ?.??, bad"; ac_verc_fail=yes;;
        1.2[6-9]* | 1.[3-9][0-9]* | [2-9].*)
changequote([,])dnl
           ac_prog_version="$ac_prog_version, ok"; ac_verc_fail=no;;
        *) ac_prog_version="$ac_prog_version, bad"; ac_verc_fail=yes;;
      esac
      AC_MSG_RESULT([$ac_prog_version])
    fi
    if test $ac_verc_fail = yes; then
      INTLBISON=:
    fi

    dnl These rules are solely for the distribution goal.  While doing this
    dnl we only have to keep exactly one list of the available catalogs
    dnl in configure.in.
    for lang in $ALL_LINGUAS; do
      GMOFILES="$GMOFILES $lang.gmo"
      POFILES="$POFILES $lang.po"
    done

    dnl Make all variables we use known to autoconf.
    AC_SUBST(BUILD_INCLUDED_LIBINTL)
    AC_SUBST(USE_INCLUDED_LIBINTL)
    AC_SUBST(CATALOGS)
    AC_SUBST(CATOBJEXT)
    AC_SUBST(GMOFILES)
    AC_SUBST(INTLLIBS)
    AC_SUBST(INTLOBJS)
    AC_SUBST(POFILES)
    AC_SUBST(POSUB)

    dnl For backward compatibility. Some configure.ins may be using this.
    nls_cv_header_intl=
    nls_cv_header_libgt=

    dnl For backward compatibility. Some Makefiles may be using this.
    DATADIRNAME=share
    AC_SUBST(DATADIRNAME)

    dnl For backward compatibility. Some Makefiles may be using this.
    INSTOBJEXT=.mo
    AC_SUBST(INSTOBJEXT)

    dnl For backward compatibility. Some Makefiles may be using this.
    GENCAT=gencat
    AC_SUBST(GENCAT)
  ])

dnl Usage: Just like AM_WITH_NLS, which see.
AC_DEFUN([AM_GNU_GETTEXT],
  [AC_REQUIRE([AC_PROG_MAKE_SET])dnl
   AC_REQUIRE([AC_PROG_CC])dnl
   AC_REQUIRE([AC_CANONICAL_HOST])dnl
   AC_REQUIRE([AC_PROG_RANLIB])dnl
   AC_REQUIRE([AC_ISC_POSIX])dnl
   AC_REQUIRE([AC_HEADER_STDC])dnl
   AC_REQUIRE([AC_C_CONST])dnl
   AC_REQUIRE([AC_C_INLINE])dnl
   AC_REQUIRE([AC_TYPE_OFF_T])dnl
   AC_REQUIRE([AC_TYPE_SIZE_T])dnl
   AC_REQUIRE([AC_FUNC_ALLOCA])dnl
   AC_REQUIRE([AC_FUNC_MMAP])dnl
   AC_REQUIRE([jm_GLIBC21])dnl

   AC_CHECK_HEADERS([argz.h limits.h locale.h nl_types.h malloc.h stddef.h \
stdlib.h string.h unistd.h sys/param.h])
   AC_CHECK_FUNCS([feof_unlocked fgets_unlocked getcwd getegid geteuid \
getgid getuid mempcpy munmap putenv setenv setlocale stpcpy strchr strcasecmp \
strdup strtoul tsearch __argz_count __argz_stringify __argz_next])

   AM_ICONV
   AM_LANGINFO_CODESET
   AM_LC_MESSAGES
   AM_WITH_NLS([$1],[$2],[$3])

   if test "x$CATOBJEXT" != "x"; then
     if test "x$ALL_LINGUAS" = "x"; then
       LINGUAS=
     else
       AC_MSG_CHECKING(for catalogs to be installed)
       NEW_LINGUAS=
       for presentlang in $ALL_LINGUAS; do
         useit=no
         for desiredlang in ${LINGUAS-$ALL_LINGUAS}; do
           # Use the presentlang catalog if desiredlang is
           #   a. equal to presentlang, or
           #   b. a variant of presentlang (because in this case,
           #      presentlang can be used as a fallback for messages
           #      which are not translated in the desiredlang catalog).
           case "$desiredlang" in
             "$presentlang"*) useit=yes;;
           esac
         done
         if test $useit = yes; then
           NEW_LINGUAS="$NEW_LINGUAS $presentlang"
         fi
       done
       LINGUAS=$NEW_LINGUAS
       AC_MSG_RESULT($LINGUAS)
     fi

     dnl Construct list of names of catalog files to be constructed.
     if test -n "$LINGUAS"; then
       for lang in $LINGUAS; do CATALOGS="$CATALOGS $lang$CATOBJEXT"; done
     fi
   fi

   dnl If the AC_CONFIG_AUX_DIR macro for autoconf is used we possibly
   dnl find the mkinstalldirs script in another subdir but $(top_srcdir).
   dnl Try to locate is.
   MKINSTALLDIRS=
   if test -n "$ac_aux_dir"; then
     MKINSTALLDIRS="$ac_aux_dir/mkinstalldirs"
   fi
   if test -z "$MKINSTALLDIRS"; then
     MKINSTALLDIRS="\$(top_srcdir)/mkinstalldirs"
   fi
   AC_SUBST(MKINSTALLDIRS)

   dnl Enable libtool support if the surrounding package wishes it.
   INTL_LIBTOOL_SUFFIX_PREFIX=ifelse([$1], use-libtool, [l], [])
   AC_SUBST(INTL_LIBTOOL_SUFFIX_PREFIX)
  ])
#serial 2
# Test for the GNU C Library, version 2.1 or newer.
# From Bruno Haible.

AC_DEFUN([jm_GLIBC21],
  [
    AC_CACHE_CHECK(whether we are using the GNU C Library 2.1 or newer,
      ac_cv_gnu_library_2_1,
      [AC_EGREP_CPP([Lucky GNU user],
	[
#include <features.h>
#ifdef __GNU_LIBRARY__
 #if (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 1) || (__GLIBC__ > 2)
  Lucky GNU user
 #endif
#endif
	],
	ac_cv_gnu_library_2_1=yes,
	ac_cv_gnu_library_2_1=no)
      ]
    )
    AC_SUBST(GLIBC21)
    GLIBC21="$ac_cv_gnu_library_2_1"
  ]
)
# Configure paths for GTK+
# Owen Taylor     1997-2001

dnl AM_PATH_GTK_2_0([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for GTK+, and define GTK_CFLAGS and GTK_LIBS, if gthread is specified in MODULES, 
dnl pass to pkg-config
dnl
AC_DEFUN([AM_PATH_GTK_2_0],
[dnl 
dnl Get the cflags and libraries from pkg-config
dnl
AC_ARG_ENABLE(gtktest, [  --disable-gtktest       do not try to compile and run a test GTK+ program],
		    , enable_gtktest=yes)

  pkg_config_args=gtk+-2.0
  for module in . $4
  do
      case "$module" in
         gthread) 
             pkg_config_args="$pkg_config_args gthread-2.0"
         ;;
      esac
  done

  no_gtk=""

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

  if test x$PKG_CONFIG != xno ; then
    if pkg-config --atleast-pkgconfig-version 0.7 ; then
      :
    else
      echo *** pkg-config too old; version 0.7 or better required.
      no_gtk=yes
      PKG_CONFIG=no
    fi
  else
    no_gtk=yes
  fi

  min_gtk_version=ifelse([$1], ,2.0.0,$1)
  AC_MSG_CHECKING(for GTK+ - version >= $min_gtk_version)

  if test x$PKG_CONFIG != xno ; then
    ## don't try to run the test against uninstalled libtool libs
    if $PKG_CONFIG --uninstalled $pkg_config_args; then
	  echo "Will use uninstalled version of GTK+ found in PKG_CONFIG_PATH"
	  enable_gtktest=no
    fi

    if $PKG_CONFIG --atleast-version $min_gtk_version $pkg_config_args; then
	  :
    else
	  no_gtk=yes
    fi
  fi

  if test x"$no_gtk" = x ; then
    GTK_CFLAGS=`$PKG_CONFIG $pkg_config_args --cflags`
    GTK_LIBS=`$PKG_CONFIG $pkg_config_args --libs`
    gtk_config_major_version=`$PKG_CONFIG --modversion gtk+-2.0 | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    gtk_config_minor_version=`$PKG_CONFIG --modversion gtk+-2.0 | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    gtk_config_micro_version=`$PKG_CONFIG --modversion gtk+-2.0 | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_gtktest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $GTK_CFLAGS"
      LIBS="$GTK_LIBS $LIBS"
dnl
dnl Now check if the installed GTK+ is sufficiently new. (Also sanity
dnl checks the results of pkg-config to some extent)
dnl
      rm -f conf.gtktest
      AC_TRY_RUN([
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.gtktest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_gtk_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_gtk_version");
     exit(1);
   }

  if ((gtk_major_version != $gtk_config_major_version) ||
      (gtk_minor_version != $gtk_config_minor_version) ||
      (gtk_micro_version != $gtk_config_micro_version))
    {
      printf("\n*** 'pkg-config --modversion gtk+-2.0' returned %d.%d.%d, but GTK+ (%d.%d.%d)\n", 
             $gtk_config_major_version, $gtk_config_minor_version, $gtk_config_micro_version,
             gtk_major_version, gtk_minor_version, gtk_micro_version);
      printf ("*** was found! If pkg-config was correct, then it is best\n");
      printf ("*** to remove the old version of GTK+. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If pkg-config was wrong, set the environment variable PKG_CONFIG_PATH\n");
      printf("*** to point to the correct configuration files\n");
    } 
  else if ((gtk_major_version != GTK_MAJOR_VERSION) ||
	   (gtk_minor_version != GTK_MINOR_VERSION) ||
           (gtk_micro_version != GTK_MICRO_VERSION))
    {
      printf("*** GTK+ header files (version %d.%d.%d) do not match\n",
	     GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     gtk_major_version, gtk_minor_version, gtk_micro_version);
    }
  else
    {
      if ((gtk_major_version > major) ||
        ((gtk_major_version == major) && (gtk_minor_version > minor)) ||
        ((gtk_major_version == major) && (gtk_minor_version == minor) && (gtk_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of GTK+ (%d.%d.%d) was found.\n",
               gtk_major_version, gtk_minor_version, gtk_micro_version);
        printf("*** You need a version of GTK+ newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** GTK+ is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the pkg-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of GTK+, but you can also set the PKG_CONFIG environment to point to the\n");
        printf("*** correct copy of pkg-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_gtk=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_gtk" = x ; then
     AC_MSG_RESULT(yes (version $gtk_config_major_version.$gtk_config_minor_version.$gtk_config_micro_version))
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$PKG_CONFIG" = "no" ; then
       echo "*** A new enough version of pkg-config was not found."
       echo "*** See http://pkgconfig.sourceforge.net"
     else
       if test -f conf.gtktest ; then
        :
       else
          echo "*** Could not run GTK+ test program, checking why..."
	  ac_save_CFLAGS="$CFLAGS"
	  ac_save_LIBS="$LIBS"
          CFLAGS="$CFLAGS $GTK_CFLAGS"
          LIBS="$LIBS $GTK_LIBS"
          AC_TRY_LINK([
#include <gtk/gtk.h>
#include <stdio.h>
],      [ return ((gtk_major_version) || (gtk_minor_version) || (gtk_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GTK+ or finding the wrong"
          echo "*** version of GTK+. If it is not finding GTK+, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GTK+ is incorrectly installed."])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GTK_CFLAGS=""
     GTK_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GTK_CFLAGS)
  AC_SUBST(GTK_LIBS)
  rm -f conf.gtktest
])
# Check whether LC_MESSAGES is available in <locale.h>.
# Ulrich Drepper <drepper@cygnus.com>, 1995.
#
# This file can be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU General Public
# License or the GNU Library General Public License but which still want
# to provide support for the GNU gettext functionality.
# Please note that the actual code of the GNU gettext library is covered
# by the GNU Library General Public License, and the rest of the GNU
# gettext package package is covered by the GNU General Public License.
# They are *not* in the public domain.

# serial 2

AC_DEFUN([AM_LC_MESSAGES],
  [if test $ac_cv_header_locale_h = yes; then
    AC_CACHE_CHECK([for LC_MESSAGES], am_cv_val_LC_MESSAGES,
      [AC_TRY_LINK([#include <locale.h>], [return LC_MESSAGES],
       am_cv_val_LC_MESSAGES=yes, am_cv_val_LC_MESSAGES=no)])
    if test $am_cv_val_LC_MESSAGES = yes; then
      AC_DEFINE(HAVE_LC_MESSAGES, 1,
        [Define if your <locale.h> file defines LC_MESSAGES.])
    fi
  fi])
#serial AM1
dnl From Bruno Haible.

AC_DEFUN([AM_LANGINFO_CODESET],
[
  AC_CACHE_CHECK([for nl_langinfo and CODESET], am_cv_langinfo_codeset,
    [AC_TRY_LINK([#include <langinfo.h>],
      [char* cs = nl_langinfo(CODESET);],
      am_cv_langinfo_codeset=yes,
      am_cv_langinfo_codeset=no)
    ])
  if test $am_cv_langinfo_codeset = yes; then
    AC_DEFINE(HAVE_LANGINFO_CODESET, 1,
      [Define if you have <langinfo.h> and nl_langinfo(CODESET).])
  fi
])

AC_DEFUN([AM_LIBCHARSET],
[
  AC_CACHE_CHECK([for libcharset], am_cv_libcharset,
    [lc_save_LIBS="$LIBS"
     LIBS="$LIBS $LIBICONV"
     AC_TRY_LINK([#include <libcharset.h>],
      [locale_charset()],
      am_cv_libcharset=yes,
      am_cv_libcharset=no) 
      LIBS="$lc_save_LIBS" 
    ])
  if test $am_cv_libcharset = yes; then
    AC_DEFINE(HAVE_LIBCHARSET, 1,
      [Define if you have <libcharset.h> and locale_charset().])
  fi
])
dnl ======================================
dnl Pidgin Configuration (formerly Gaim)
dnl ======================================
dnl
dnl Call: AC_PIDGIN(major, minor, micro)
dnl Returns: $have_pidgin (= yes/no), $have_purple (= yes/no)
dnl
dnl Copyright (C) 2004 - 2007 Josef Spillner <josef@ggzgamingzone.org>
dnl

AC_DEFUN([AC_PIDGIN],
[
AC_ARG_ENABLE([pidgin_plugin],
  AS_HELP_STRING([--disable-pidgin-plugin], [Do not compile pidgin plugin]),
  [enable_pidgin_plugin=no],
  [enable_pidgin_plugin=yes])

if test "$enable_pidgin_plugin" = "yes"; then
  AC_PURPLE_CHECK([$1], [$2], [$3])
  if test "$have_purple" = yes; then
    AC_PIDGIN_CHECK
  fi
else
  have_pidgin=no
fi
])

AC_DEFUN([AC_PURPLE_CHECK],
[
AC_MSG_CHECKING([for Pidgin library: libpurple])

ac_purple_includes=NO
purple_includes=""

AC_CACHE_VAL(ac_cv_have_purple,
[
AC_GGZ_FIND_FILE(libpurple/plugin.h, "/usr/include", purple_incdir)
ac_purple_includes="$purple_incdir"

if test "$ac_purple_includes" = NO; then
  ac_cv_have_purple="have_purple=no"
else
  have_purple="yes"
fi
])

eval "$ac_cv_have_purple"

if test "$have_purple" != yes; then
  AC_MSG_RESULT([$have_purple])
else
  AC_MSG_RESULT([$have_purple (headers $ac_purple_includes)])

  major=$1
  minor=$2
  micro=$3

  testprologue=""
  testprologue="$testprologue #include <glib/gtypes.h>"
  testprologue="$testprologue #include <libpurple/version.h>"
  testbody=""
  testbody="$testbody if(!purple_version_check($major, $minor, $micro)) return 0;"
  testbody="$testbody else return -1;"

  save_cppflags=$CPPFLAGS
  save_libs=$LIBS
  glibflags=`pkg-config --cflags glib-2.0 2>/dev/null`
  CPPFLAGS="$CPPFLAGS $glibflags"
  LIBS="$LIBS -lpurple"
  AC_RUN_IFELSE(
    [AC_LANG_PROGRAM([[$testprologue]], [[$testbody]])],
    [],
    [
      AC_MSG_WARN([The Pidgin/purple version is too old. Version $major.$minor.$micro is required.])
	  have_purple=no
    ]
  )
  CPPFLAGS=$save_cppflags
  LIBS=$save_LIBS
fi
])

AC_DEFUN([AC_PIDGIN_CHECK],
[
AC_MSG_CHECKING([for Pidgin library: libpidgin])

ac_pidgin_includes=NO
pidgin_includes=""

AC_CACHE_VAL(ac_cv_have_pidgin,
[
AC_GGZ_FIND_FILE(pidgin/gtkplugin.h, "/usr/include", pidgin_incdir)
ac_pidgin_includes="$pidgin_incdir"

if test "$ac_pidgin_includes" = NO; then
  ac_cv_have_pidgin="have_pidgin=no"
else
  have_pidgin="yes"
fi
])

eval "$ac_cv_have_pidgin"

if test "$have_pidgin" != yes; then
  AC_MSG_RESULT([$have_pidgin])
else
  AC_MSG_RESULT([$have_pidgin (headers $ac_pidgin_includes)])
fi
])

# Search path for a program which passes the given test.
# Ulrich Drepper <drepper@cygnus.com>, 1996.
#
# This file can be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU General Public
# License or the GNU Library General Public License but which still want
# to provide support for the GNU gettext functionality.
# Please note that the actual code of the GNU gettext library is covered
# by the GNU Library General Public License, and the rest of the GNU
# gettext package package is covered by the GNU General Public License.
# They are *not* in the public domain.

# serial 2

dnl AM_PATH_PROG_WITH_TEST(VARIABLE, PROG-TO-CHECK-FOR,
dnl   TEST-PERFORMED-ON-FOUND_PROGRAM [, VALUE-IF-NOT-FOUND [, PATH]])
AC_DEFUN([AM_PATH_PROG_WITH_TEST],
[# Extract the first word of "$2", so it can be a program name with args.
set dummy $2; ac_word=[$]2
AC_MSG_CHECKING([for $ac_word])
AC_CACHE_VAL(ac_cv_path_$1,
[case "[$]$1" in
  /*)
  ac_cv_path_$1="[$]$1" # Let the user override the test with a path.
  ;;
  *)
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:"
  for ac_dir in ifelse([$5], , $PATH, [$5]); do
    test -z "$ac_dir" && ac_dir=.
    if test -f $ac_dir/$ac_word; then
      if [$3]; then
	ac_cv_path_$1="$ac_dir/$ac_word"
	break
      fi
    fi
  done
  IFS="$ac_save_ifs"
dnl If no 4th arg is given, leave the cache variable unset,
dnl so AC_PATH_PROGS will keep looking.
ifelse([$4], , , [  test -z "[$]ac_cv_path_$1" && ac_cv_path_$1="$4"
])dnl
  ;;
esac])dnl
$1="$ac_cv_path_$1"
if test ifelse([$4], , [-n "[$]$1"], ["[$]$1" != "$4"]); then
  AC_MSG_RESULT([$]$1)
else
  AC_MSG_RESULT(no)
fi
AC_SUBST($1)dnl
])
# Check for the presence of C99 features.  Generally the check will fail
# if the feature isn't present (a C99 compiler isn't that much to ask,
# right?).

# Copyright (C) 2004-2005 Jason Short.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# Originally written by Jason Short for Freeciv.  Imported and updated for
# GGZ.  The canonical copy of this resides in the libggz module.
# $Id: c99.m4 9076 2007-04-30 07:55:48Z josef $

# Check C99-style variadic macros (required):
#
#  #define PRINTF(msg, ...) (printf(msg, __VA_ARGS__)
#
AC_DEFUN([AC_C99_VARIADIC_MACROS],
[
  dnl Check for variadic macros
  AC_CACHE_CHECK([for C99 variadic macros],
    [ac_cv_c99_variadic_macros],
     [AC_TRY_COMPILE(
          [#include <stdio.h>
           #define MSG(...) fprintf(stderr, __VA_ARGS__)
          ],
          [MSG("foo");
           MSG("%s", "foo");
           MSG("%s%d", "foo", 1);],
          ac_cv_c99_variadic_macros=yes,
          ac_cv_c99_variadic_macros=no)])
  if test "x${ac_cv_c99_variadic_macros}" != "xyes"; then
    AC_MSG_ERROR([A compiler supporting C99 variadic macros is required])
  fi
])

# Check C99-style variable-sized arrays (required):
#
#   char concat_str[strlen(s1) + strlen(s2) + 1];
#
AC_DEFUN([AC_C99_VARIABLE_ARRAYS],
[
  dnl Check for variable arrays
  AC_CACHE_CHECK([for C99 variable arrays],
    [ac_cv_c99_variable_arrays],
    [AC_TRY_COMPILE(
        [],
        [char *s1 = "foo", *s2 = "bar";
         char s3[strlen(s1) + strlen(s2) + 1];
         sprintf(s3, "%s%s", s1, s2);],
        ac_cv_c99_variable_arrays=yes,
        ac_cv_c99_variable_arrays=no)])
  if test "x${ac_cv_c99_variable_arrays}" != "xyes"; then
    AC_MSG_ERROR([A compiler supporting C99 variable arrays is required])
  fi
])

# Check C99-style initializers (required):
#
# Examples:
#   struct timeval tv = {.tv_sec = 0, .tv_usec = 500000};
#   int fibonacci[6] = {[0] = 0, [1] = 1, [2] = 1, [3] = 2, [4] = 3, [5] = 5};
# Note we do not check for multi-field initializers like
#   struct { struct { int b; } a; } = {.a.b = 5}
# which are not supported by many compilers.  It is best to avoid this
# problem by writing these using nesting.  The above case becomes
#   struct { struct { int b; } a; } = {.a = {.b = 5}}
AC_DEFUN([AC_C99_INITIALIZERS],
[
  dnl Check for C99 initializers
  AC_CACHE_CHECK([for C99 initializers],
    [ac_cv_c99_initializers],
    [AC_TRY_COMPILE(
        [struct foo {
           int an_integer;
           char *a_string;
           int an_array[5];
           union {int x, y;} a_union;
         };
        ],
        [struct foo bar = {.an_array = {0, [3] = 2, [2] = 1, [4] = 3},
                           .an_integer = 999,
                           .a_string = "does it work?",
                           .a_union = {.y = 243}};
         bar.an_integer = 998;],
        [ac_cv_c99_initializers=yes],
        [ac_cv_c99_initializers=no])])
  if test "${ac_cv_c99_initializers}" != "yes"; then
    AC_MSG_ERROR([A compiler supporting C99 initializers is required])
  fi
])

# Check C99-style stdint.h (required)
AC_DEFUN([AC_C99_STDINT_H],
[
  AC_CHECK_HEADERS([stdint.h])
  dnl Check for C99 stdint.h
  AC_CACHE_CHECK([for C99 stdint.h],
    [ac_cv_c99_stdint_h],
    [ac_cv_c99_stdint_h=$ac_cv_header_stdint_h])
  if test "${ac_cv_c99_stdint_h}" != "yes"; then
    AC_MSG_ERROR([A compiler supporting C99's stdint.h is required])
  fi
])
# AC_GGZ_DEBUG
# $Id: ggz-debug.m4 9779 2008-03-07 17:09:08Z josef $
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
    LIBS="$LIBS -ldmalloc"
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
dnl ======================================
dnl GGZ Gaming Zone - Configuration Macros
dnl ======================================
dnl
dnl Copyright (C) 2001 - 2007 Josef Spillner, josef@ggzgamingzone.org
dnl This file has heavily been inspired by KDE's acinclude :)
dnl It is published under the conditions of the GNU General Public License.
dnl
dnl ======================================
dnl
dnl This file is common to most GGZ modules, and should be kept in sync
dnl between them all.  The master copy resides with libggz.
dnl Currently the following modules use it:
dnl   kde-games, kde-client, gtk-games, gtk-client, utils, grubby,
dnl   ggz-client-libs, ggzd, gnome-client, txt-client, sdl-games, libggz
dnl See /docs/ggz-project/buildsystem for documentation.
dnl
dnl ======================================
dnl
dnl History:
dnl   See the SVN log for a full history.
dnl
dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl High-level macros:
dnl   AC_GGZ_CHECK - Checks for presence of GGZ client and server libraries.
dnl                  GGZ users can call this macro to determine at compile
dnl                  time whether to include GGZ support.  Server and client
dnl                  are checked separately.  GGZ_SERVER and GGZ_CLIENT are
dnl                  defined in config.h, and created as conditionals in
dnl                  the Makefiles.
dnl   AC_GGZ_CHECK_SERVER - The same, but server libs only.
dnl
dnl Low-level macros:
dnl   AC_GGZ_INIT - initialization and paths/options setup
dnl   AC_GGZ_VERSION - ensure a minimum version of GGZ
dnl   AC_GGZ_LIBGGZ - find the libggz headers and libraries
dnl   AC_GGZ_GGZCORE - find the ggzcore headers and libraries
dnl   AC_GGZ_CONFIG - find the ggz-config tool and set up configuration
dnl   AC_GGZ_GGZMOD - find the ggzmod library
dnl   AC_GGZ_GGZDMOD - find the ggzdmod library
dnl   AC_GGZ_SERVER - set up game and room path for ggzd game servers
dnl   AC_GGZ_VISIBILITY - linker support for symbol visibility
dnl
dnl   Each macro takes two arguments:
dnl     1.  Action-if-found (or empty for no action).
dnl     2.  Action-if-not-found (or empty for error, or "ignore" to ignore).
dnl
dnl Internal functions:
dnl   AC_GGZ_ERROR - user-friendly error messages
dnl   AC_GGZ_FIND_FILE - macro for convenience (thanks kde)
dnl   AC_GGZ_REMOVEDUPS - eliminate duplicate list elements
dnl

# Version number of this script
# serial 0015

dnl ------------------------------------------------------------------------
dnl Find a directory containing a single file
dnl Synopsis: AC_GGZ_FIND_FILE(file, directorylist, <returnvar>)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_FIND_FILE],
[
$3=NO
for i in $2;
do
  for j in $1;
  do
    echo "configure: __oline__: $i/$j" >&AC_FD_CC
    if test -r "$i/$j"; then
      echo "taking that" >&AC_FD_CC
      $3=$i
      break 2
    fi
  done
done
])

dnl ------------------------------------------------------------------------
dnl Remove duplicate entries in a list, and remove all NO's
dnl Synopsis: AC_GGZ_REMOVEDUPS(list, <returnlist>)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_REMOVEDUPS],
[
ret=""
for i in $1; do
  add=yes
  for j in $ret; do
    if test "x$i" = "x$j"; then
      add=no
    fi
  done
  if test "x$i" = "xNO"; then
    add=no
  fi
  if test "x$add" = "xyes"; then
  ret="$ret $i"
  fi
done
$2=$ret
])

dnl ------------------------------------------------------------------------
dnl User-friendly error messages
dnl Synopsis: AC_GGZ_ERROR(libraryname, headerdirlist, libdirlist)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_ERROR],
[
  AC_MSG_WARN([no
  The library '$1' does not seem to be installed correctly.
  Headers searched in: $2
  Libraries searched in: $3
  Please read QuickStart.GGZ in order to fix this.
  ])
  exit 1
])

dnl ------------------------------------------------------------------------
dnl Initialization, common values and such
dnl Synopsis: AC_GGZ_INIT([export], [defaults])
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_INIT],
[
if test "x$prefix" = "xNONE"; then
  prefix="${ac_default_prefix}"
fi
if test "x$exec_prefix" = "xNONE"; then
  exec_prefix='${prefix}'
fi

ac_ggz_prefix=""
AC_ARG_WITH(ggz-dir,
    AC_HELP_STRING([--with-ggz-dir=DIR], [Path to GGZ Gaming Zone]),
    [  ac_ggz_prefix="$withval"
    ])

if test "x${prefix}" = "xNONE"; then
   ac_ggz_prefix_incdir="${ac_default_prefix}/include"
   ac_ggz_prefix_libdir="${ac_default_prefix}/lib"
   ac_ggz_prefix_bindir="${ac_default_prefix}/bin"
   ac_ggz_prefix_etcdir="${ac_default_prefix}/etc"
else
   unq_includedir="${includedir}"
   unq_libdir="${libdir}"
   unq_bindir="${bindir}"
   unq_sysconfdir="${sysconfdir}"

   eval unq_includedir=`echo $unq_includedir`
   eval unq_includedir=`echo $unq_includedir`
   eval unq_libdir=`echo $unq_libdir`
   eval unq_libdir=`echo $unq_libdir`
   eval unq_bindir=`echo $unq_bindir`
   eval unq_bindir=`echo $unq_bindir`
   eval unq_sysconfdir=`echo $unq_sysconfdir`
   eval unq_sysconfdir=`echo $unq_sysconfdir`

   ac_ggz_prefix_incdir="${unq_includedir}"
   ac_ggz_prefix_libdir="${unq_libdir}"
   ac_ggz_prefix_bindir="${unq_bindir}"
   ac_ggz_prefix_etcdir="${unq_sysconfdir}"
fi
ac_ggz_stdinc="$ac_ggz_prefix_incdir"
ac_ggz_stdlib="$ac_ggz_prefix_libdir"
ac_ggz_stdbin="$ac_ggz_prefix_bindir"
ac_ggz_stdetc="$ac_ggz_prefix_etcdir/ggzd"
if test "x$ac_ggz_prefix" != "x"; then
  ac_ggz_stdinc="$ac_ggz_stdinc $ac_ggz_prefix/include"
  ac_ggz_stdlib="$ac_ggz_stdlib $ac_ggz_prefix/lib $ac_ggz_prefix/lib64"
  ac_ggz_stdbin="$ac_ggz_stdbin $ac_ggz_prefix/bin"
  ac_ggz_stdetc="$ac_ggz_stdetc $ac_ggz_prefix/etc/ggzd"
fi
if test "x$1" = "xdefaults" || test "x$2" = "xdefaults"; then
  ac_ggz_stdinc="$ac_ggz_stdinc /usr/local/include /usr/include"
  ac_ggz_stdlib="$ac_ggz_stdlib /usr/local/lib /usr/local/lib64 /usr/lib /usr/lib64"
  ac_ggz_stdbin="$ac_ggz_stdbin /usr/local/bin /usr/bin"
  ac_ggz_stdetc="$ac_ggz_stdetc /usr/local/etc/ggzd /etc/ggzd"
fi
if test "x$1" = "xexport" || test "x$2" = "xexport"; then
  CPPFLAGS="$CPPFLAGS -I ${ac_ggz_prefix_incdir} -I /usr/local/include"
  LDFLAGS="$LDFLAGS -L${ac_ggz_prefix_libdir} -L/usr/local/lib"
fi

save_cflags=$CFLAGS
save_cxxflags=$CXXFLAGS
if test "x$GCC" = xyes; then
	CFLAGS="-Wall -Werror"
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
		[[void signedness(void){char c;if(c==-1)c=0;}]])],
		[],
		[save_cflags="$save_cflags -fsigned-char"
		 save_cxxflags="$save_cxxflags -fsigned-char"])
else
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
		[[#if defined(__SUNPRO_C) || (__SUNPRO_C >= 0x550)
		#else
		# include "Error: Only GCC and Sun Studio are supported compilers."
		#endif]], [[]])],
		[save_cflags="$save_cflags -xchar=signed"
		 save_cxxflags="$save_cxxflags -xchar=signed"],
		[])

fi
CFLAGS=$save_cflags
CXXFLAGS=$save_cxxflags
])

dnl ------------------------------------------------------------------------
dnl Experimental symbol visibility for the linker
dnl Synopsis: AC_GGZ_VISIBILITY
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_VISIBILITY],
[
	AC_ARG_ENABLE([visibility],
		AS_HELP_STRING([--disable-visibility],[Don't restrict library symbol visibility]),
		[enable_visibility=$enableval], [enable_visibility="yes"])
	AM_CONDITIONAL(SYMBOL_VISIBILITY, test "$enable_visibility" = yes)
])

dnl ------------------------------------------------------------------------
dnl Ensure that a minimum version of GGZ is present
dnl Synopsis: AC_GGZ_VERSION(major, minor, micro,
dnl                          action-if-found, action-if-not-found)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_VERSION],
[
	major=$1
	minor=$2
	micro=$3

	testprologue="#include <ggz.h>"
	testbody=""
	testbody="$testbody if(LIBGGZ_VERSION_MAJOR > $major) return 0;"
	testbody="$testbody if(LIBGGZ_VERSION_MAJOR < $major) return -1;"
	testbody="$testbody if(LIBGGZ_VERSION_MINOR > $minor) return 0;"
	testbody="$testbody if(LIBGGZ_VERSION_MINOR < $minor) return -1;"
	testbody="$testbody if(LIBGGZ_VERSION_MICRO > $micro) return 0;"
	testbody="$testbody if(LIBGGZ_VERSION_MICRO < $micro) return -1;"
	testbody="$testbody return 0;"

	save_libs=$LIBS
	save_ldflags=$LDFLAGS
	save_cppflags=$CPPFLAGS
	save_ldlibrary_path=$LD_LIBRARY_PATH
	LDFLAGS=$LIBGGZ_LDFLAGS
	LIBS=$LIB_GGZ
	CPPFLAGS=$LIBGGZ_INCLUDES
	export LD_LIBRARY_PATH=$save_ldlibrary_path:$libggz_libraries

	AC_MSG_CHECKING([for GGZ library version: $major.$minor.$micro])
	AC_RUN_IFELSE(
		[AC_LANG_PROGRAM([[$testprologue]], [[$testbody]])],
		[ac_ggz_version_check=yes],
		[ac_ggz_version_check=no],
		[ac_ggz_version_check="skipped due to cross-compiling"]
	)
	if test "$ac_ggz_version_check" = "no"; then
		AC_MSG_RESULT([no])
		if test "x$5" = "x"; then
			AC_MSG_ERROR([The GGZ version is too old. Version $major.$minor.$micro is required.])
		fi
		$5
	else
		AC_MSG_RESULT($ac_ggz_version_check)
		$4
	fi

	LIBS=$save_libs
	LDFLAGS=$save_ldflags
	CPPFLAGS=$save_cppflags
	LD_LIBRARY_PATH=$save_ldlibrary_path
])

dnl ------------------------------------------------------------------------
dnl Try to find the libggz headers and libraries.
dnl $(LIBGGZ_LDFLAGS) will be -L ... (if needed)
dnl and $(LIBGGZ_INCLUDES) will be -I ... (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_LIBGGZ],
[
AC_MSG_CHECKING([for GGZ library: libggz])

ac_libggz_includes=NO ac_libggz_libraries=NO
libggz_libraries=""
libggz_includes=""

AC_ARG_WITH(libggz-dir,
    AC_HELP_STRING([--with-libggz-dir=DIR],[libggz installation prefix]),
    [  ac_libggz_includes="$withval"/include
       ac_libggz_libraries="$withval"/lib
    ])
AC_ARG_WITH(libggz-includes,
    AC_HELP_STRING([--with-libggz-includes=DIR],
                   [where the libggz includes are]),
    [  ac_libggz_includes="$withval"
    ])
AC_ARG_WITH(libggz-libraries,
    AC_HELP_STRING([--with-libggz-libraries=DIR],[where the libggz libs are]),
    [  ac_libggz_libraries="$withval"
    ])

AC_CACHE_VAL(ac_cv_have_libggz,
[
libggz_incdirs="$ac_libggz_includes $ac_ggz_stdinc"
AC_GGZ_REMOVEDUPS($libggz_incdirs, libggz_incdirs)
libggz_header=ggz.h

AC_GGZ_FIND_FILE($libggz_header, $libggz_incdirs, libggz_incdir)
ac_libggz_includes="$libggz_incdir"

libggz_libdirs="$ac_libggz_libraries $ac_ggz_stdlib"
AC_GGZ_REMOVEDUPS($libggz_libdirs, libggz_libdirs)

libggz_libdir=NO
for dir in $libggz_libdirs; do
  try="ls -1 $dir/libggz.la $dir/libggz.so"
  if test -n "`$try 2> /dev/null`"; then libggz_libdir=$dir; break; else echo "tried $dir" >&AC_FD_CC ; fi
done

ac_libggz_libraries="$libggz_libdir"

if test "$ac_libggz_includes" = NO || test "$ac_libggz_libraries" = NO; then
  ac_cv_have_libggz="have_libggz=no"
  ac_libggz_notfound=""
else
  have_libggz="yes"
fi
])

eval "$ac_cv_have_libggz"

if test "$have_libggz" != yes; then
  if test "x$2" = "xignore"; then
    AC_MSG_RESULT([$have_libggz (ignored)])
  else
    AC_MSG_RESULT([$have_libggz])
    if test "x$2" = "x"; then
      AC_GGZ_ERROR(libggz, $libggz_incdirs, $libggz_libdirs)
    fi

    # perform actions given by argument 2.
    $2
  fi
else
  ac_cv_have_libggz="have_libggz=yes \
    ac_libggz_includes=$ac_libggz_includes ac_libggz_libraries=$ac_libggz_libraries"
  AC_MSG_RESULT([$have_libggz (libraries $ac_libggz_libraries, headers $ac_libggz_includes)])

  libggz_libraries="$ac_libggz_libraries"
  libggz_includes="$ac_libggz_includes"

  AC_SUBST(libggz_libraries)
  AC_SUBST(libggz_includes)

  LIBGGZ_INCLUDES="-I $libggz_includes"
  LIBGGZ_LDFLAGS="-L$libggz_libraries"

  AC_SUBST(LIBGGZ_INCLUDES)
  AC_SUBST(LIBGGZ_LDFLAGS)

  LIB_GGZ='-lggz'
  AC_SUBST(LIB_GGZ)

  # perform actions given by argument 1.
  $1
fi

])

dnl ------------------------------------------------------------------------
dnl Try to find the ggzcore headers and libraries.
dnl $(GGZCORE_LDFLAGS) will be -L ... (if needed)
dnl and $(GGZCORE_INCLUDES) will be -I ... (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_GGZCORE],
[
AC_MSG_CHECKING([for GGZ library: ggzcore])

ac_ggzcore_includes=NO ac_ggzcore_libraries=NO
ggzcore_libraries=""
ggzcore_includes=""

AC_ARG_WITH(ggzcore-dir,
    AC_HELP_STRING([--with-ggzcore-dir=DIR],[ggzcore installation prefix]),
    [  ac_ggzcore_includes="$withval"/include
       ac_ggzcore_libraries="$withval"/lib
    ])
AC_ARG_WITH(ggzcore-includes,
    AC_HELP_STRING([--with-ggzcore-includes=DIR],
                   [where the ggzcore includes are]),
    [  ac_ggzcore_includes="$withval"
    ])
AC_ARG_WITH(ggzcore-libraries,
    AC_HELP_STRING([--with-ggzcore-libraries=DIR],
                   [where the ggzcore libs are]),
    [  ac_ggzcore_libraries="$withval"
    ])

AC_CACHE_VAL(ac_cv_have_ggzcore,
[
ggzcore_incdirs="$ac_ggzcore_includes $ac_ggz_stdinc"
AC_GGZ_REMOVEDUPS($ggzcore_incdirs, ggzcore_incdirs)
ggzcore_header=ggzcore.h

AC_GGZ_FIND_FILE($ggzcore_header, $ggzcore_incdirs, ggzcore_incdir)
ac_ggzcore_includes="$ggzcore_incdir"

ggzcore_libdirs="$ac_ggzcore_libraries $ac_ggz_stdlib"
AC_GGZ_REMOVEDUPS($ggzcore_libdirs, ggzcore_libdirs)

ggzcore_libdir=NO
for dir in $ggzcore_libdirs; do
  try="ls -1 $dir/libggzcore.la $dir/libggzcore.so"
  if test -n "`$try 2> /dev/null`"; then ggzcore_libdir=$dir; break; else echo "tried $dir" >&AC_FD_CC ; fi
done

ac_ggzcore_libraries="$ggzcore_libdir"

if test "$ac_ggzcore_includes" = NO || test "$ac_ggzcore_libraries" = NO; then
  ac_cv_have_ggzcore="have_ggzcore=no"
  ac_ggzcore_notfound=""
else
  have_ggzcore="yes"
fi
])

eval "$ac_cv_have_ggzcore"

if test "$have_ggzcore" != yes; then
  if test "x$2" = "xignore"; then
    AC_MSG_RESULT([$have_ggzcore (intentionally ignored)])
  else
    AC_MSG_RESULT([$have_ggzcore])
    if test "x$2" = "x"; then
      AC_GGZ_ERROR(ggzcore, $ggzcore_incdirs, $ggzcore_libdirs)
    fi

    # Perform actions given by argument 2.
    $2
  fi
else
  ac_cv_have_ggzcore="have_ggzcore=yes \
    ac_ggzcore_includes=$ac_ggzcore_includes ac_ggzcore_libraries=$ac_ggzcore_libraries"
  AC_MSG_RESULT([$have_ggzcore (libraries $ac_ggzcore_libraries, headers $ac_ggzcore_includes)])

  ggzcore_libraries="$ac_ggzcore_libraries"
  ggzcore_includes="$ac_ggzcore_includes"

  AC_SUBST(ggzcore_libraries)
  AC_SUBST(ggzcore_includes)

  GGZCORE_INCLUDES="-I $ggzcore_includes"
  GGZCORE_LDFLAGS="-L$ggzcore_libraries"

  AC_SUBST(GGZCORE_INCLUDES)
  AC_SUBST(GGZCORE_LDFLAGS)

  LIB_GGZCORE='-lggzcore'
  AC_SUBST(LIB_GGZCORE)

  # Perform actions given by argument 1.
  $1
fi

])

dnl ------------------------------------------------------------------------
dnl Try to find the ggz-config binary.
dnl Sets GGZ_CONFIG to the path/name of the program.
dnl Sets also: ggz_gamedir, ggz_datadir etc.
dnl Takes a 3rd, optional parameter: [registry]
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_CONFIG],
[
AC_MSG_CHECKING([for GGZ configuration tool: ggz-config])

ac_ggz_config=NO
ggz_config=""

AC_ARG_WITH(ggzconfig,
    AC_HELP_STRING([--with-ggzconfig=DIR],[path to ggz-config]),
    [  ac_ggz_config="$withval"
    ])

ac_ggz_config_orig=$ac_ggz_config

AC_CACHE_VAL(ac_cv_have_ggzconfig,
[
ggz_config_dirs="$ac_ggz_config $ac_ggz_stdbin"

AC_GGZ_FIND_FILE(ggz-config, $ggz_config_dirs, ggz_config_dir)
ac_ggz_config="$ggz_config_dir"

if test "$ac_ggz_config" = NO; then
  ac_cv_have_ggzcore="have_ggz_config=no"
  ac_ggz_config_notfound=""
  have_ggz_config="no"
else
  have_ggz_config="yes"
fi
])

eval "$ac_cv_have_ggz_config"

if test "$have_ggz_config" != yes; then
  if test "x$2" = "xignore"; then
    AC_MSG_RESULT([$have_ggz_config (intentionally ignored)])
    GGZ_CONFIG="true"
    ggzexecmoddir="\${libdir}/ggz"
    ggzdatadir="\${datadir}/ggz"
    AC_SUBST(GGZ_CONFIG)
    AC_SUBST(ggzexecmoddir)
    AC_SUBST(ggzdatadir)
    AC_DEFINE_UNQUOTED(GAMEDIR, "${libdir}/ggz", [Path where to install the games])
    AC_DEFINE_UNQUOTED(GGZDATADIR, "${datadir}/ggz", [Path where the games should look for their data files])
  else
    AC_MSG_RESULT([$have_ggz_config])
    if test "x$2" = "x"; then
      AC_MSG_ERROR([ggz-config not found. Please check your installation! ])
    fi

    # Perform actions given by argument 2.
    $2
  fi
else
  pathto_app=`echo $prefix/bin/ | tr -s "/"`
  pathto_ggz=`echo $ac_ggz_config/ | tr -s "/"`

  if test "$ac_ggz_config_orig" != "NO"; then
    pathto_app=$pathto_ggz
  fi

  if test "x$pathto_app" != "x$pathto_ggz"; then
    AC_MSG_RESULT([$have_ggz_config (dismissed due to different prefix)])
    GGZ_CONFIG="true"
    ggzexecmoddir="\${libdir}/ggz"
    ggzdatadir="\${datadir}/ggz"
    AC_SUBST(GGZ_CONFIG)
    AC_SUBST(ggzexecmoddir)
    AC_SUBST(ggzdatadir)
    AC_DEFINE_UNQUOTED(GGZMODULECONFDIR, "${sysconfdir}", [Path where the game registry is located])
    AC_DEFINE_UNQUOTED(GAMEDIR, "${libdir}/ggz", [Path where to install the games])
    AC_DEFINE_UNQUOTED(GGZDATADIR, "${datadir}/ggz", [Path where the games should look for their data files])
  else
    ac_cv_have_ggz_config="have_ggz_config=yes \
      ac_ggz_config=$ac_ggz_config"
    AC_MSG_RESULT([$ac_ggz_config/ggz-config])

    ggz_config="$ac_ggz_config"
    AC_SUBST(ggz_config)

    AC_ARG_ENABLE([noregistry],
      AC_HELP_STRING([--enable-noregistry], [Do not register game modules.]),
      [enable_noregistry=yes], [enable_noregistry=no])

    GGZ_CONFIG="${ggz_config}/ggz-config"
    if test "$enable_noregistry" = yes; then
      GGZ_CONFIG="$GGZ_CONFIG --noregistry=$enableval"
    fi
    if test "$3" != ""; then
      GGZ_CONFIG="$GGZ_CONFIG --registry=$3"
    fi
    AC_SUBST(GGZ_CONFIG)

    ggzmoduleconfdir=`$GGZ_CONFIG --configdir`
    AC_DEFINE_UNQUOTED(GGZMODULECONFDIR, "${ggzmoduleconfdir}", [Path where the game registry is located])
    ggzexecmoddir=`$GGZ_CONFIG --gamedir`
    AC_DEFINE_UNQUOTED(GAMEDIR, "${ggzexecmoddir}", [Path where to install the games])
    ggzdatadir=`$GGZ_CONFIG --datadir`
    AC_DEFINE_UNQUOTED(GGZDATADIR, "${ggzdatadir}", [Path where the games should look for their data files])
    packagesrcdir=`cd $srcdir && pwd`
    AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Path where the source is located])

    if test "$ggzmoduleconfdir" = ""; then
      AC_MSG_ERROR([ggz-config is unusable. Maybe LD_LIBRARY_PATH needs to be set.])
    fi

    AC_SUBST(ggzmoduleconfdir)
    AC_SUBST(ggzexecmoddir)
    AC_SUBST(ggzdatadir)
    AC_SUBST(packagesrcdir)

    # Perform actions given by argument 1.
    $1
  fi
fi

])

dnl ------------------------------------------------------------------------
dnl Try to find the ggzmod headers and libraries.
dnl $(GGZMOD_LDFLAGS) will be -L ... (if needed)
dnl and $(GGZMOD_INCLUDES) will be -I ... (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_GGZMOD],
[
AC_MSG_CHECKING([for GGZ library: ggzmod])

ac_ggzmod_includes=NO ac_ggzmod_libraries=NO
ggzmod_libraries=""
ggzmod_includes=""

AC_ARG_WITH(ggzmod-dir,
    AC_HELP_STRING([--with-ggzmod-dir=DIR],[ggzmod installation prefix]),
    [  ac_ggzmod_includes="$withval"/include
       ac_ggzmod_libraries="$withval"/lib
    ])
AC_ARG_WITH(ggzmod-includes,
    AC_HELP_STRING([--with-ggzmod-includes=DIR],
                   [where the ggzmod includes are]),
    [  ac_ggzmod_includes="$withval"
    ])
AC_ARG_WITH(ggzmod-libraries,
    AC_HELP_STRING([--with-ggzmod-libraries=DIR],
                   [where the ggzmod libs are]),
    [  ac_ggzmod_libraries="$withval"
    ])

AC_CACHE_VAL(ac_cv_have_ggzmod,
[
ggzmod_incdirs="$ac_ggzmod_includes $ac_ggz_stdinc"
AC_GGZ_REMOVEDUPS($ggzmod_incdirs, ggzmod_incdirs)
ggzmod_header=ggzmod.h

AC_GGZ_FIND_FILE($ggzmod_header, $ggzmod_incdirs, ggzmod_incdir)
ac_ggzmod_includes="$ggzmod_incdir"

ggzmod_libdirs="$ac_ggzmod_libraries $ac_ggz_stdlib"
AC_GGZ_REMOVEDUPS($ggzmod_libdirs, ggzmod_libdirs)

ggzmod_libdir=NO
for dir in $ggzmod_libdirs; do
  try="ls -1 $dir/libggzmod.la $dir/libggzmod.so"
  if test -n "`$try 2> /dev/null`"; then ggzmod_libdir=$dir; break; else echo "tried $dir" >&AC_FD_CC ; fi
done

ac_ggzmod_libraries="$ggzmod_libdir"

if test "$ac_ggzmod_includes" = NO || test "$ac_ggzmod_libraries" = NO; then
  ac_cv_have_ggzmod="have_ggzmod=no"
  ac_ggzmod_notfound=""
else
  have_ggzmod="yes"
fi
])

eval "$ac_cv_have_ggzmod"

if test "$have_ggzmod" != yes; then
  if test "x$2" = "xignore"; then
    AC_MSG_RESULT([$have_ggzmod (intentionally ignored)])
  else
    AC_MSG_RESULT([$have_ggzmod])
    if test "x$2" = "x"; then
      AC_GGZ_ERROR(ggzmod, $ggzmod_incdirs, $ggzmod_libdirs)
    fi

    # Perform actions given by argument 2.
    $2
  fi
else
  ac_cv_have_ggzmod="have_ggzmod=yes \
    ac_ggzmod_includes=$ac_ggzmod_includes ac_ggzmod_libraries=$ac_ggzmod_libraries"
  AC_MSG_RESULT([$have_ggzmod (libraries $ac_ggzmod_libraries, headers $ac_ggzmod_includes)])

  ggzmod_libraries="$ac_ggzmod_libraries"
  ggzmod_includes="$ac_ggzmod_includes"

  AC_SUBST(ggzmod_libraries)
  AC_SUBST(ggzmod_includes)

  GGZMOD_INCLUDES="-I $ggzmod_includes"
  GGZMOD_LDFLAGS="-L$ggzmod_libraries"

  AC_SUBST(GGZMOD_INCLUDES)
  AC_SUBST(GGZMOD_LDFLAGS)

  LIB_GGZMOD='-lggzmod'
  AC_SUBST(LIB_GGZMOD)

  # Perform actions given by argument 1.
  $1
fi

])

dnl ------------------------------------------------------------------------
dnl Try to find the ggzdmod headers and libraries.
dnl $(GGZDMOD_LDFLAGS) will be -L ... (if needed)
dnl and $(GGZDMOD_INCLUDES) will be -I ... (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_GGZDMOD],
[
AC_MSG_CHECKING([for GGZ library: ggzdmod])

ac_ggzdmod_includes=NO ac_ggzdmod_libraries=NO
ggzdmod_libraries=""
ggzdmod_includes=""

AC_ARG_WITH(ggzdmod-dir,
    AC_HELP_STRING([--with-ggzdmod-dir=DIR], [ggzdmod installation prefix]),
    [  ac_ggzdmod_includes="$withval"/include
       ac_ggzdmod_libraries="$withval"/lib
    ])
AC_ARG_WITH(ggzdmod-includes,
    AC_HELP_STRING([--with-ggzdmod-includes=DIR], 
                   [where the ggzdmod includes are]),
    [  ac_ggzdmod_includes="$withval"
    ])
AC_ARG_WITH(ggzdmod-libraries,
    AC_HELP_STRING([--with-ggzdmod-libraries=DIR],
                   [where the ggzdmod libs are]),
    [  ac_ggzdmod_libraries="$withval"
    ])

AC_CACHE_VAL(ac_cv_have_ggzdmod,
[
ggzdmod_incdirs="$ac_ggzdmod_includes $ac_ggz_stdinc"
AC_GGZ_REMOVEDUPS($ggzdmod_incdirs, ggzdmod_incdirs)
ggzdmod_header=ggzdmod.h

AC_GGZ_FIND_FILE($ggzdmod_header, $ggzdmod_incdirs, ggzdmod_incdir)
ac_ggzdmod_includes="$ggzdmod_incdir"

ggzdmod_libdirs="$ac_ggzdmod_libraries $ac_ggz_stdlib"
AC_GGZ_REMOVEDUPS($ggzdmod_libdirs, ggzdmod_libdirs)

ggzdmod_libdir=NO
for dir in $ggzdmod_libdirs; do
  try="ls -1 $dir/libggzdmod.la $dir/libggzdmod.so"
  if test -n "`$try 2> /dev/null`"; then ggzdmod_libdir=$dir; break; else echo "tried $dir" >&AC_FD_CC ; fi
done

ac_ggzdmod_libraries="$ggzdmod_libdir"

if test "$ac_ggzdmod_includes" = NO || test "$ac_ggzdmod_libraries" = NO; then
  ac_cv_have_ggzdmod="have_ggzdmod=no"
  ac_ggzdmod_notfound=""
else
  have_ggzdmod="yes"
fi
])

eval "$ac_cv_have_ggzdmod"

if test "$have_ggzdmod" != yes; then
  if test "x$2" = "xignore"; then
    AC_MSG_RESULT([$have_ggzdmod (intentionally ignored)])
  else
    AC_MSG_RESULT([$have_ggzdmod])
    if test "x$2" = "x"; then
      AC_GGZ_ERROR(ggzdmod, $ggzdmod_incdirs, $ggzdmod_libdirs)
    fi

    # Perform actions given by argument 2.
    $2
  fi
else
  ac_cv_have_ggzdmod="have_ggzdmod=yes \
    ac_ggzdmod_includes=$ac_ggzdmod_includes ac_ggzdmod_libraries=$ac_ggzdmod_libraries"
  AC_MSG_RESULT([$have_ggzdmod (libraries $ac_ggzdmod_libraries, headers $ac_ggzdmod_includes)])

  ggzdmod_libraries="$ac_ggzdmod_libraries"
  ggzdmod_includes="$ac_ggzdmod_includes"

  AC_SUBST(ggzdmod_libraries)
  AC_SUBST(ggzdmod_includes)

  GGZDMOD_INCLUDES="-I $ggzdmod_includes"
  GGZDMOD_LDFLAGS="-L$ggzdmod_libraries"

  AC_SUBST(GGZDMOD_INCLUDES)
  AC_SUBST(GGZDMOD_LDFLAGS)

  LIB_GGZDMOD='-lggzdmod'
  AC_SUBST(LIB_GGZDMOD)

  # Perform actions given by argument 1.
  $1
fi

])

dnl ------------------------------------------------------------------------
dnl Try to find the ggz-gtk headers and libraries.
dnl $(GGZGTK_LDFLAGS) will be -L ... (if needed)
dnl and $(GGZGTK_INCLUDES) will be -I ... (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_GTK],
[
AC_MSG_CHECKING([for GGZ library: ggz-gtk])

ac_ggz_gtk_includes=NO ac_ggz_gtk_libraries=NO
ggz_gtk_libraries=""
ggz_gtk_includes=""

AC_ARG_WITH(ggz-gtk-dir,
    AC_HELP_STRING([--with-ggz-gtk-dir=DIR], [ggz-gtk installation prefix]),
    [  ac_ggz_gtk_includes="$withval"/include
       ac_ggz_gtk_libraries="$withval"/lib
    ])
AC_ARG_WITH(ggz-gtk-includes,
    AC_HELP_STRING([--with-ggz-gtk-includes=DIR], 
                   [where the ggz-gtk includes are]),
    [  ac_ggz_gtk_includes="$withval"
    ])
AC_ARG_WITH(ggz-gtk-libraries,
    AC_HELP_STRING([--with-ggz-gtk-libraries=DIR],
                   [where the ggz-gtk libs are]),
    [  ac_ggz_gtk_libraries="$withval"
    ])

AC_CACHE_VAL(ac_cv_have_ggz_gtk,
[
ggz_gtk_incdirs="$ac_ggz_gtk_includes $ac_ggz_stdinc"
AC_GGZ_REMOVEDUPS($ggz_gtk_incdirs, ggz_gtk_incdirs)
ggz_gtk_header=ggz-gtk.h

AC_GGZ_FIND_FILE($ggz_gtk_header, $ggz_gtk_incdirs, ggz_gtk_incdir)
ac_ggz_gtk_includes="$ggz_gtk_incdir"

ggz_gtk_libdirs="$ac_ggz_gtk_libraries $ac_ggz_stdlib"
AC_GGZ_REMOVEDUPS($ggz_gtk_libdirs, ggz_gtk_libdirs)

ggz_gtk_libdir=NO
for dir in $ggz_gtk_libdirs; do
  try="ls -1 $dir/libggz-gtk.la $dir/libggz-gtk.so"
  if test -n "`$try 2> /dev/null`"; then ggz_gtk_libdir=$dir; break; else echo "tried $dir" >&AC_FD_CC ; fi
done

ac_ggz_gtk_libraries="$ggz_gtk_libdir"

if test "$ac_ggz_gtk_includes" = NO || test "$ac_ggz_gtk_libraries" = NO; then
  ac_cv_have_ggz_gtk="have_ggz_gtk=no"
  ac_ggz_gtk_notfound=""
else
  have_ggz_gtk="yes"
fi
])

eval "$ac_cv_have_ggz_gtk"

if test "$have_ggz_gtk" != yes; then
  if test "x$2" = "xignore"; then
    AC_MSG_RESULT([$have_ggz_gtk (intentionally ignored)])
  else
    AC_MSG_RESULT([$have_ggz_gtk])
    if test "x$2" = "x"; then
      AC_GGZ_ERROR(ggz-gtk, $ggz_gtk_incdirs, $ggz_gtk_libdirs)
    fi

    # Perform actions given by argument 2.
    $2
  fi
else
  ac_cv_have_ggz_gtk="have_ggz_gtk=yes \
    ac_ggz_gtk_includes=$ac_ggz_gtk_includes ac_ggz_gtk_libraries=$ac_ggz_gtk_libraries"
  AC_MSG_RESULT([$have_ggz_gtk (libraries $ac_ggz_gtk_libraries, headers $ac_ggz_gtk_includes)])

  ggz_gtk_libraries="$ac_ggz_gtk_libraries"
  ggz_gtk_includes="$ac_ggz_gtk_includes"

  AC_SUBST(ggz_gtk_libraries)
  AC_SUBST(ggz_gtk_includes)

  GGZ_GTK_INCLUDES="-I $ggz_gtk_includes"
  GGZ_GTK_LDFLAGS="-L$ggz_gtk_libraries"

  AC_SUBST(GGZ_GTK_INCLUDES)
  AC_SUBST(GGZ_GTK_LDFLAGS)

  LIB_GGZ_GTK='-lggz-gtk'
  AC_SUBST(LIB_GGZ_GTK)

  # Perform actions given by argument 1.
  $1
fi
])

dnl ------------------------------------------------------------------------
dnl Setup the game server configuration.
dnl Sets ggzdconfdir (ggzd configuration).
dnl Sets ggzddatadir (for game server data).
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_SERVER],
[
AC_MSG_CHECKING([for GGZ server: ggzd])
AC_ARG_WITH(ggzd-confdir,
    AC_HELP_STRING([--with-ggzd-confdir=DIR], [directory for room/game data]),
[ ac_ggzd_confdir="$withval"
])

AC_CACHE_VAL(ac_cv_have_ggzdconf,
[
	if test "x$1" = "xforce"; then
		if test "x$ac_ggzd_confdir" = "x"; then
			ggzdconfdirs="$ac_ggz_stdetc"
		else
			ggzdconfdirs="$ac_ggzd_confdir"
		fi
	else
		ggzdconfdirs="$ac_ggzd_confdir $ac_ggz_stdetc"
	fi

	ggzdconfdir=NONE
	for dir in $ggzdconfdirs; do
		if test -n "`ls -d $dir/rooms 2> /dev/null`"; then
			if test -n "`ls -d $dir/rooms 2> /dev/null`"; then
				ggzdconfdir=$dir; break;
			else
				echo "tried $dir" >&AC_FD_CC;
			fi
		else
			echo "tried $dir" >&AC_FD_CC;
		fi
	done

	if test "x$ggzdconfdir" = "xNONE"; then
		have_ggzdconf="no"
	else
		have_ggzdconf="yes"
	fi
])

eval "$ac_cv_have_ggzdconf"

if test "$have_ggzdconf" != yes; then
	if test "x$2" = "xignore"; then
	  AC_MSG_RESULT([$have_ggzdconf (intentionally ignored)])
	elif test "x$2" = "xforce"; then
	  if test "x$ac_ggzd_confdir" = "x"; then
	    ggzdconfdir="\${sysconfdir}/ggzd"
	  else
	    ggzdconfdir=$ac_ggzd_confdir
	  fi
	  AC_MSG_RESULT([$have_ggzdconf (but forced to ${ggzdconfdir})])
	else
	  AC_MSG_RESULT([$have_ggzdconf])
	  if test "x$2" = "x"; then
	    AC_MSG_ERROR([GGZ server configuration not found. Please check your installation! ])
	  fi

	  # Perform actions given by argument 2.
	  $2
	fi
else
	prefixed=0
	if test "x${prefix}" != "xNONE" && test "x${prefix}" != "x${ac_default_prefix}"; then
		if test "x$ac_ggzd_confdir" = "x"; then
			prefixed=1
		fi
	fi
	if test "x$ggzdconfdir" != "x${sysconfdir}/ggzd" && test "x$prefixed" = "x1"; then
		AC_MSG_RESULT([$have_ggzdconf ($ggzdconfdir, but using ${sysconfdir}/ggzd nevertheless)])
		ggzdconfdir="\${sysconfdir}/ggzd"
	else
		AC_MSG_RESULT([$have_ggzdconf ($ggzdconfdir)])
	fi
fi

if test "$have_ggzdconf" = yes || test "x$2" = "xforce"; then
	AC_SUBST(ggzdconfdir)

	ggzddatadir=${datadir}/${PACKAGE}
	AC_SUBST(ggzddatadir)

	if test "x${libdir}" = 'x${exec_prefix}/lib'; then
	  if test "x${exec_prefix}" = "xNONE"; then
	    if test "x${prefix}" = "xNONE"; then
	      ggzdexecmoddir="\${ac_default_prefix}/lib/ggzd"
	      ggzdexecmodpath="${ac_default_prefix}/lib/ggzd"
	    else
	      ggzdexecmoddir="\${prefix}/lib/ggzd"
	      ggzdexecmodpath="${prefix}/lib/ggzd"
	    fi
	  else
	    ggzdexecmoddir="\${exec_prefix}/lib/ggzd"
	    ggzdexecmodpath="${exec_prefix}/lib/ggzd"
	  fi
	else
	  ggzdexecmoddir="\${libdir}/ggzd"
	  ggzdexecmodpath="${libdir}/ggzd"
	fi
	AC_SUBST(ggzdexecmoddir)
	AC_SUBST(ggzdexecmodpath)

	# Perform actions given by argument 1.
	$1
fi

])

dnl ------------------------------------------------------------------------
dnl AC_GGZ_CHECK_SERVER
dnl   Check for presence of GGZ server libraries.
dnl
dnl   Simply call this function in programs that use GGZ.  GGZ_SERVER will
dnl   be #defined in config.h, and created as a conditional
dnl   in Makefile.am files, if server libraries are present.
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_CHECK_SERVER],
[
  AC_GGZ_LIBGGZ([try_ggz="yes"], [try_ggz="no"])
  if test "$try_ggz" = "yes"; then
    # For now, version 0.0.14 is required.  This could be an additional
    # parameter.
    AC_GGZ_VERSION([0], [99], [4], [], [try_ggz=no])
  fi

  ggz_server="no"
  AC_ARG_WITH(ggz-server,
              AC_HELP_STRING([--with-ggz-server], [Force GGZ server support]),
              [try_ggz_server=$withval])

  if test "x$try_ggz_server" != "xno"; then
    if test "$try_ggz" = "yes"; then
      # Must pass something as the action-if-failed, or the macro will exit
      AC_GGZ_GGZDMOD([ggz_server="yes"], [ggz_server="no"])
    fi
    if test "$ggz_server" = "yes"; then
      AC_GGZ_SERVER([force],[force])
      AC_DEFINE(GGZ_SERVER, 1, [Server support for GGZ])
    else
      if test "$try_ggz_server" = "yes"; then
        AC_MSG_ERROR([Could not configure GGZ server support. See above messages.])
      fi
    fi
  fi

  AM_CONDITIONAL(GGZ_SERVER, test "$ggz_server" = "yes")
])

dnl ------------------------------------------------------------------------
dnl AC_GGZ_CHECK
dnl   Check for presence of GGZ client and server libraries.
dnl
dnl   Simply call this function in programs that use GGZ.  GGZ_SERVER and
dnl   GGZ_CLIENT will be #defined in config.h, and created as conditionals
dnl   in Makefile.am files.
dnl
dnl   The only argument accepted gives the frontend for client embedding:
dnl      "gtk" => means the libggz-gtk library will be checked
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_CHECK],
[
  AC_GGZ_INIT([defaults])
  AC_GGZ_LIBGGZ([try_ggz="yes"], [try_ggz="no"])

  if test "$try_ggz" = "yes"; then
    # For now, version 0.0.14 is required.  This could be an additional
    # parameter.
    AC_GGZ_VERSION([0], [0], [14], [], [try_ggz=no])
  fi

  ggz_client="no"
  AC_ARG_WITH(ggz-client,
              AC_HELP_STRING([--with-ggz-client], [Force GGZ client support]),
              [try_ggz_client=$withval])

  if test "x$try_ggz_client" != "xno"; then
    if test "$try_ggz" = "yes"; then
      # Must pass something as the action-if-failed, or the macro will exit
      AC_GGZ_GGZMOD([AC_GGZ_CONFIG([ggz_client="yes"], [ggz_client="no"])],
                    [ggz_client="no"])
    fi
    if test "$ggz_client" = "yes"; then
      AC_DEFINE(GGZ_CLIENT, 1, [Client support for GGZ])
    else
      if test "$try_ggz_client" = "yes"; then
        AC_MSG_ERROR([Could not configure GGZ client support. See above messages.])
      fi
    fi
  fi

  ggz_gtk="no"
  if test "$ggz_client" = "yes"; then
    if test "x$1" = "xgtk"; then
      AC_GGZ_GTK([ggz_gtk="yes"])
      if test $ggz_gtk = "yes"; then
        AC_DEFINE(GGZ_GTK, 1, [Support for embedded GGZ through libggz-gtk])
      fi
    fi
  fi

  AM_CONDITIONAL(GGZ_CLIENT, test "$ggz_client" = "yes")
  AM_CONDITIONAL(GGZ_GTK, test "$ggz_gtk" = "yes")

  AC_GGZ_CHECK_SERVER
])
#serial AM2
dnl From Bruno Haible.

AC_DEFUN([AM_ICONV],
[
  dnl Some systems have iconv in libc, some have it in libiconv (OSF/1 and
  dnl those with the standalone portable GNU libiconv installed).

  AC_ARG_WITH([libiconv-prefix],
[  --with-libiconv-prefix=DIR  search for libiconv in DIR/include and DIR/lib], [
    for dir in `echo "$withval" | tr : ' '`; do
      if test -d $dir/include; then CPPFLAGS="$CPPFLAGS -I$dir/include"; fi
      if test -d $dir/lib; then LDFLAGS="$LDFLAGS -L$dir/lib"; fi
    done
   ])

  AC_CACHE_CHECK(for iconv, am_cv_func_iconv, [
    am_cv_func_iconv="no, consider installing GNU libiconv"
    am_cv_lib_iconv=no
    AC_TRY_LINK([#include <stdlib.h>
#include <iconv.h>],
      [iconv_t cd = iconv_open("","");
       iconv(cd,NULL,NULL,NULL,NULL);
       iconv_close(cd);],
      am_cv_func_iconv=yes)
    if test "$am_cv_func_iconv" != yes; then
      am_save_LIBS="$LIBS"
      LIBS="$LIBS -liconv"
      AC_TRY_LINK([#include <stdlib.h>
#include <iconv.h>],
        [iconv_t cd = iconv_open("","");
         iconv(cd,NULL,NULL,NULL,NULL);
         iconv_close(cd);],
        am_cv_lib_iconv=yes
        am_cv_func_iconv=yes)
      LIBS="$am_save_LIBS"
    fi
  ])
  if test "$am_cv_func_iconv" = yes; then
    AC_DEFINE(HAVE_ICONV, 1, [Define if you have the iconv() function.])
    AC_MSG_CHECKING([for iconv declaration])
    AC_CACHE_VAL(am_cv_proto_iconv, [
      AC_TRY_COMPILE([
#include <stdlib.h>
#include <iconv.h>
extern
#ifdef __cplusplus
"C"
#endif
#if defined(__STDC__) || defined(__cplusplus)
size_t iconv (iconv_t cd, char * *inbuf, size_t *inbytesleft, char * *outbuf, size_t *outbytesleft);
#else
size_t iconv();
#endif
], [], am_cv_proto_iconv_arg1="", am_cv_proto_iconv_arg1="const")
      am_cv_proto_iconv="extern size_t iconv (iconv_t cd, $am_cv_proto_iconv_arg1 char * *inbuf, size_t *inbytesleft, char * *outbuf, size_t *outbytesleft);"])
    am_cv_proto_iconv=`echo "[$]am_cv_proto_iconv" | tr -s ' ' | sed -e 's/( /(/'`
    AC_MSG_RESULT([$]{ac_t:-
         }[$]am_cv_proto_iconv)
    AC_DEFINE_UNQUOTED(ICONV_CONST, $am_cv_proto_iconv_arg1,
      [Define as const if the declaration of iconv() needs const.])
  fi
  LIBICONV=
  if test "$am_cv_lib_iconv" = yes; then
    LIBICONV="-liconv"
  fi
  AC_SUBST(LIBICONV)
])
# $Id: mkdir.m4 8809 2007-01-04 20:38:26Z jdorje $
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
dnl ======================================
dnl GGZ Gaming Zone - Configuration Macros
dnl ======================================
dnl
dnl Copyright (C) 2005, 2006 Josef Spillner, josef@ggzgamingzone.org
dnl This file has been derived from libggz configuration macros.
dnl It is published under the conditions of the GNU General Public License.
dnl
dnl ======================================
dnl
dnl This file contains operating system specific checks such as for system
dnl libraries, header file flavours and non-POSIX/Unix handling.
dnl Macros defined herein are allowed to modify LIBS and LDFLAGS.
dnl Optional (advanced) checks go to system.m4 and should use LIB_* variables.
dnl
dnl ======================================
dnl
dnl History:
dnl 2005-09-14: created from libggz's configure.ac file

dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_PLATFORM_BASE - minimal required call
dnl AC_GGZ_PLATFORM_POSIX - checks for various types and functions
dnl AC_GGZ_PLATFORM_POSIX_NET - network-related checks
dnl AC_GGZ_PLATFORM_POSIX_LIBC - system libraries in addition to libc
dnl AC_GGZ_PLATFORM_WIN32 - for cross-compilation on win32 systems
dnl AC_GGZ_PLATFORM - convenience macro to call all of the above
dnl   (except for _BASE which MUST be called before AC_PROG_LIBTOOL)
dnl

dnl ------------------------------------------------------------------------
dnl AC_GGZ_PLATFORM_BASE
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_PLATFORM_BASE],
[
AC_PROG_CC
#AC_PROG_CXX # not needed for libggz itself
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_CPP
AC_LIBTOOL_WIN32_DLL
#AC_PROG_LIBTOOL # bug!
AC_PROG_RANLIB
AC_PROG_MAKE_SET
])

dnl ------------------------------------------------------------------------
dnl AC_GGZ_PLATFORM_WIN32
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_PLATFORM_WIN32],
[
  case $host_os in
    *mingw32* )
      MINGW32=yes
      ;;
    * )
      MINGW32=no
      ;;
  esac

  if test "$MINGW32" = "yes"; then
    LDFLAGS="$LDFLAGS -no-undefined"
    LIBS="$LIBS -lws2_32"
  fi

  AC_CHECK_HEADERS([winsock2.h])
])

dnl ------------------------------------------------------------------------
dnl AC_GGZ_PLATFORM_POSIX
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_PLATFORM_POSIX],
[
# Check for header files
# ======================
#AC_CHECK_HEADERS([fcntl.h sys/param.h],
#  [],
#  [AC_MSG_ERROR([cannot find required header file])])
#AC_CHECK_HEADERS([netdb.h sys/shm.h sys/socket.h])
#AC_CHECK_HEADERS([sys/uio.h sys/un.h sys/wait.h])
AC_CHECK_HEADERS([arpa/inet.h sys/types.h netinet/in.h])

# Check for functions
# ===================
#AC_CHECK_FUNCS([getpagesize memmove memset mkdir stat strcasecmp strerror strstr strchr],
#  [],
#  [AC_MSG_ERROR([cannot find required function])])
#AC_CHECK_FUNCS([gethostbyname socket]) # These fail; dunno why
#AC_CHECK_FUNCS([alarm getuid inet_pton])

# Check for more functions
# ========================
#AC_FUNC_FORK
##AC_FUNC_MALLOC # use ggz_malloc() always
#AC_FUNC_MEMCMP
#AC_FUNC_REALLOC
#AC_FUNC_STAT
FUNC_MKDIR_TAKES_ONE_ARG # FIXME: external

# Check for typedefs, structures, and compiler characteristics
# ============================================================
#AC_C_CONST
#AC_TYPE_MODE_T
#AC_TYPE_PID_T
#AC_TYPE_SIGNAL
#AC_TYPE_SIZE_T
AC_C99_VARIABLE_ARRAYS # FIXME: external

# Check for debug types in syslog.h
# =================================
# If syslog is present, we take the log types from it.
# If not, define our own enumeration.
AC_CHECK_HEADER([syslog.h],
  [
    AC_DEFINE([GGZ_HAVE_SYSLOG_H], 1, [Check if syslog is present])
  ],
  [])
])

dnl ------------------------------------------------------------------------
dnl AC_GGZ_PLATFORM_POSIX_NET
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_PLATFORM_POSIX_NET],
[
# Check for sendmsg
# =================
# If sendmsg() is present, we can send file descriptors over a local socket
# via ggz_write_fd.  If not we disable this functionality.
#AC_MSG_CHECKING([for network feature: sendmsg])
AC_CHECK_FUNC([sendmsg],
  [
    AC_DEFINE([GGZ_HAVE_SENDMSG], 1, [Check if file descriptors can be sent])
  ],
  [])

# Check for PF_LOCAL/PF_UNIX
# ==========================
# There should be an autoconf macro to check this???
AC_MSG_CHECKING([for network feature: PF_LOCAL])
have_localsockets=0
AC_COMPILE_IFELSE(
  AC_LANG_PROGRAM([[#include <sys/socket.h>]], [[int a = PF_LOCAL;]]),
  [have_localsockets=1],
  [
    AC_COMPILE_IFELSE(
      AC_LANG_PROGRAM([[#include <sys/socket.h>]], [[int a = PF_UNIX;]]),
      [
        AC_DEFINE([PF_LOCAL], PF_UNIX, [PF_LOCAL is available via PF_UNIX])
        AC_DEFINE([AF_LOCAL], AF_UNIX, [AF_LOCAL is available via AF_UNIX])
        have_localsockets=1
      ],
      [])
  ])
if test "x$have_localsockets" = "x1"; then
  AC_DEFINE([GGZ_HAVE_PF_LOCAL], 1, [Check if local sockets are supported])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

# Check for SUN_LEN
# =================
AC_MSG_CHECKING([for network feature: SUN_LEN])
AC_LINK_IFELSE(
  AC_LANG_PROGRAM(
    [[
      #include<sys/types.h>
      #include<sys/un.h>
    ]],
    [[struct sockaddr_un su; int i = SUN_LEN(&su);]]),
  [
    AC_MSG_RESULT(yes)
    AC_DEFINE([HAVE_SUN_LEN], 1, [Define if the SUN_LEN macro exists])
  ],
  [AC_MSG_RESULT(no)])

# Check for msghdr member msg_controllen and friends
# ==================================================
AC_MSG_CHECKING([for network feature: msg_controllen])
have_controllen=0
AC_COMPILE_IFELSE(
  AC_LANG_PROGRAM([[#include <sys/socket.h>]],
    [[struct msghdr m; m.msg_controllen = 0;]]),
  [have_controllen=1],
  [])

if test "x$have_controllen" = "x1"; then
  AC_DEFINE([HAVE_MSGHDR_MSG_CONTROL], 1, [Define if msghdr has a msg_controllen member])
  AC_MSG_RESULT(yes)

  AC_MSG_CHECKING([for network feature: CMSG_ALIGN])
  AC_LINK_IFELSE(
    AC_LANG_PROGRAM(
      [[
        #include<sys/types.h>
        #include<sys/socket.h>
      ]],
      [[int i = CMSG_ALIGN(0);]]),
    [
      AC_MSG_RESULT(yes)
      AC_DEFINE([HAVE_CMSG_ALIGN], 1, [Define if CMSG_ALIGN is available])
    ],
    [AC_MSG_RESULT(no)])

  AC_MSG_CHECKING([for network feature: CMSG_LEN])
  AC_LINK_IFELSE(
    AC_LANG_PROGRAM(
      [[
        #include<sys/types.h>
        #include<sys/socket.h>
      ]],
      [[int i = CMSG_LEN(0);]]),
    [
      AC_MSG_RESULT(yes)
      AC_DEFINE([HAVE_CMSG_LEN], 1, [Define if CMSG_LEN is available])
    ],
    [AC_MSG_RESULT(no)])

  AC_MSG_CHECKING([for network feature: CMSG_SPACE])
  AC_LINK_IFELSE(
    AC_LANG_PROGRAM(
      [[
        #include<sys/types.h>
        #include<sys/socket.h>
      ]],
      [[int i = CMSG_SPACE(0);]]),
    [
      AC_MSG_RESULT(yes)
      AC_DEFINE([HAVE_CMSG_SPACE], 1, [Define if CMSG_SPACE is available])
    ],
    [AC_MSG_RESULT(no)])
else
  AC_MSG_RESULT(no)
fi
])

dnl ------------------------------------------------------------------------
dnl AC_GGZ_PLATFORM_POSIX_LIBC
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_PLATFORM_POSIX_LIBC],
[
# Check for library functions
# ===========================
# On some systems additional libraries may be needed.  Simply include them
# if they're there.
AC_CHECK_LIB(socket, socket, [LIBS="$LIBS -lsocket"])
AC_CHECK_LIB(nsl, gethostbyname, [LIBS="$LIBS -lnsl"])
])

dnl ------------------------------------------------------------------------
dnl AC_GGZ_PLATFORM
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_PLATFORM],
[
#AC_GGZ_PLATFORM_BASE
AC_GGZ_PLATFORM_POSIX
AC_GGZ_PLATFORM_POSIX_NET
AC_GGZ_PLATFORM_POSIX_LIBC
AC_GGZ_PLATFORM_WIN32
])

dnl ======================================
dnl GGZ Gaming Zone - Configuration Macros
dnl ======================================
dnl
dnl Copyright (C) 2006 Josef Spillner, josef@ggzgamingzone.org
dnl It is published under the conditions of the GNU General Public License.
dnl
dnl ======================================
dnl
dnl This file contains system library dependent macros such as asynchronous
dnl hostname lookups.
dnl As opposed to platform.m4, all optional features go here.
dnl
dnl ======================================
dnl

dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_ASYNC - find out if libanl or libasyncns is available which makes
dnl                resolving non-blocking
dnl                -> called automatically by AC_GGZ_PTHREADS
dnl AC_GGZ_PTHREADS - look for a pthreads implementation
dnl AC_GGZ_INTL - ensure proper i18n tools installation
dnl               -> this requires iconv.m4!
dnl

dnl ------------------------------------------------------------------------
dnl Check if asynchronous hostname lookups are possible
dnl Automake variables: $(LIB_ASYNC)
dnl Autoconf variables: $enable_anl
dnl Defines: GAI_A or ASYNCNS
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_ASYNC],
[
  AC_ARG_ENABLE([anl],
    AC_HELP_STRING([--enable-anl], [Enable asynchronous hostname lookups]),
    [enable_anl=$enableval],
    [enable_anl=no])

  if test $enable_anl != "no"; then
    AC_CHECK_LIB(asyncns, asyncns_new,
      [AC_DEFINE([ASYNCNS], 1, [Support for asynchronous lookups with asyncns])
       enable_anl=yes
       LIB_ASYNC="-lasyncns"],
      [enable_anl=next])
  fi

  if test $enable_anl = "next"; then
    AC_CHECK_LIB(anl, getaddrinfo_a,
      [AC_DEFINE([GAI_A], 1, [Support for asynchronous hostname lookups])
       enable_anl=yes
       LIB_ASYNC="-lanl"],
      [enable_anl=no])
  fi

  AC_SUBST(LIB_ASYNC)
])

dnl ------------------------------------------------------------------------
dnl Check for pthreads
dnl Automake variables: $(LIB_PTHREADS)
dnl Autoconf variables: $enable_threading
dnl Defines: NO_THREADING
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_PTHREADS],
[
AC_ARG_ENABLE([threading],
  AS_HELP_STRING([--disable-threading], [Don't build in threading support]),
  [enable_threading=$enableval],
  [enable_threading=yes])

if test $enable_threading != "no"; then
  # Threading support is not available on all platforms.  It's dangerous
  # to disable it so this must be done explicitly.
  AC_CHECK_HEADER(pthread.h, [],
                  AC_MSG_ERROR([*** Cannot find pthread.h header]))
  AC_CHECK_LIB(pthread, pthread_create,
               [LIB_PTHREADS="-lpthread"
                CFLAGS="$CFLAGS -D_REENTRANT"],
               [AC_CHECK_LIB(c_r, pthread_create,
                             [LIB_PTHREADS="-pthread"
                              CFLAGS="$CFLAGS -D_THREAD_SAFE"],
               AC_MSG_ERROR([*** Cannot find pthread library]))])

  # If threading is available, maybe asynchronous hostname lookups
  # are as well, using libanl from glibc >= 2.3.
  AC_GGZ_ASYNC
else
  AC_DEFINE([NO_THREADING], 1, [Define if threading support is disabled])
fi

  AC_SUBST(LIB_PTHREADS)
])

dnl ------------------------------------------------------------------------
dnl Find internationalization tools
dnl Automake variables: $(LIB_GETTEXT),
dnl   $(XGETTEXT), $(GETTEXT), $(GMSGFMT), $(MSGFMT), $(MSGMERGE)
dnl Autoconf variables: (none)
dnl Defines: ENABLE_NLS
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_GGZ_INTL],
[
AC_PATH_PROG(GETTEXT, xgettext)
AC_PATH_PROG(MSGFMT, msgfmt)
AC_PATH_PROG(MSGMERGE, msgmerge)

intl=1
if test "x$GETTEXT" = "x"; then intl=0; fi
if test "x$MSGFMT" = "x"; then intl=0; fi
if test "x$MSGMERGE" = "x"; then intl=0; fi
AM_ICONV
AC_CHECK_LIB(intl, gettext, [LIB_GETTEXT="-lintl $LIBICONV"])
save_libs=$LIBS
LIBS="$LIB_GETTEXT"
save_cflags=$CFLAGS
CFLAGS="$CFLAGS -fno-builtin"
AC_CHECK_FUNCS([gettext ngettext], [], [intl=0])
LIBS=$save_libs
CFLAGS=$save_cflags
AC_CHECK_HEADERS([libintl.h locale.h])
if test "$intl" = 0; then
  if test "x$2" = "xignore"; then
    AC_MSG_WARN([Internationalization tools missing. (ignored)])
  else
    AC_MSG_RESULT([Internationalization tools missing.])
    if test "x$2" = "x"; then
      AC_MSG_ERROR([Internationalization tools missing.])
    fi

    # Perform actions given by argument 2.
    $2
  fi
else
  AC_MSG_RESULT([Internationalization tools found.])

  XGETTEXT=$GETTEXT
  GMSGFMT=$MSGFMT

  AC_SUBST(XGETTEXT)
  AC_SUBST(GETTEXT)
  AC_SUBST(GMSGFMT)
  AC_SUBST(MSGFMT)
  AC_SUBST(MSGMERGE)

  AC_SUBST(LIB_GETTEXT)

  AC_DEFINE(ENABLE_NLS, 1, [Define if NLS is enabled])

  # Perform actions given by argument 1.
  $1
fi

])

