dnl Process this file with autoconf to produce a configure script.

AC_INIT([GGZ chat bot], [0.0.10pre], [ggz-dev@mail.ggzgamingzone.org], [ggz-grubby])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl Compiling Options
dnl =====================
AC_ARG_ENABLE(perl,  [  --disable-perl        Exclude perl modules],, enable_perl=yes)
AC_ARG_ENABLE(ruby,  [  --disable-ruby        Exclude ruby modules],, enable_ruby=yes)
AC_ARG_ENABLE(python,[  --disable-python      Exclude python modules],, enable_python=yes)

dnl Check for standard build environment
dnl ====================================
AC_PROG_CC
AC_ISC_POSIX
AC_PROG_INSTALL
AM_PROG_CC_STDC
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

dnl GGZ checks
dnl ==========
AC_GGZ_INIT(defaults)
AC_GGZ_INTL
AC_GGZ_LIBGGZ
AC_GGZ_GGZCORE
AC_GGZ_GGZMOD("true", ignore)
AC_GGZ_CONFIG

SUBDIRS="grubby man"

if test "x$ggzmod_includes" != "x"; then
	AC_DEFINE([GAMES], 1, [Compile internal game engines])
	SUBDIRS="$SUBDIRS games"
	enable_games="yes"
else
	enable_games="no"
fi

dnl Use -Wall if we have gcc.
dnl ========================
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl


dnl Module options
dnl ===========
if test "$enable_perl" = yes; then
	echo -n "Extra check: perl... "

	inc=`perl -MConfig -e 'print "$Config{archlibexp}/CORE"' 2>/dev/null`
	incs="$inc /usr/lib/perl/5.8.0/CORE /usr/lib/perl/5.6.1/CORE /usr/lib/perl/5.6.0/CORE"
	AC_GGZ_FIND_FILE("perl.h", $incs, perl_incdir)
	perl_includes="$perl_incdir"

	libs="/usr/local/lib /usr/lib"
	AC_GGZ_FIND_FILE("libperl.so", $libs, perl_libdir)
	perl_libraries="$perl_libdir"

	if test "x$perl_includes" = "xNO" || test "x$perl_libraries" = "xNO"; then
		AC_MSG_RESULT(no.)
		perl_includes=""
		perl_libraries=""
		enable_perl=no
	else
		#perl_includes="-I $perl_includes"
		#perl_libraries="-L $perl_libraries -lperl"
		perl_includes=`perl -MExtUtils::Embed -e ccopts`
		perl_libraries=`perl -MExtUtils::Embed -e ldopts`
		AC_MSG_RESULT(ok.)
		AC_SUBST(perl_includes)
		AC_SUBST(perl_libraries)
		AC_DEFINE([EMBED_PERL], 1, [Define to use embedded Perl])
	fi
fi

if test "$enable_ruby" = yes; then
	echo -n "Extra check: ruby... "

	incs=`ruby -r rbconfig -e 'printf("%s", Config::CONFIG@<:@"archdir"@:>@)' 2>/dev/null`
	libs=`ruby -r rbconfig -e 'printf("%s", Config::CONFIG@<:@"libdir"@:>@)' 2>/dev/null`
	rver=`ruby -r rbconfig -e 'printf("%s", Config::CONFIG@<:@"ruby_install_name"@:>@)' 2>/dev/null`

	AC_GGZ_FIND_FILE("ruby.h", $incs, ruby_incdir)
	ruby_includes="$ruby_incdir"

	AC_GGZ_FIND_FILE("libruby.so", $libs, ruby_libdir)
	ruby_libraries="$ruby_libdir"
	if test "x$ruby_libraries" = "xNO"; then
		AC_GGZ_FIND_FILE("lib$rver.so", $libs, ruby_libdir)
		ruby_libraries="$ruby_libdir"
	else
		rver="ruby"
	fi

	if test "x$ruby_includes" = "xNO" || test "x$ruby_libraries" = "xNO"; then
		AC_MSG_RESULT(no.)
		ruby_includes=""
		ruby_libraries=""
		enable_ruby=no
	else
		ruby_includes="-I $ruby_includes"
		ruby_libraries="-L $ruby_libraries -l$rver"
		AC_MSG_RESULT(ok.)
		AC_SUBST(ruby_includes)
		AC_SUBST(ruby_libraries)
		AC_DEFINE([EMBED_RUBY], 1, [Define to use embedded Ruby])
	fi
fi

if test "$enable_python" = yes; then
	echo -n "Extra check: python... "

	usever=""
	for ver in 2.2 2.3 2.4; do
		incs="/usr/local/include/python$ver /usr/include/python$ver"
		AC_GGZ_FIND_FILE("Python.h", $incs, python_incdir)
		python_includes="$python_incdir"

		libs="/usr/local/lib /usr/lib"
		AC_GGZ_FIND_FILE(libpython$ver.so, $libs, python_libdir)
		python_libraries="$python_libdir"

		if test "x$python_includes" = "xNO" || test "x$python_libraries" = "xNO"; then
			python_includes=""
			python_libraries=""
			enable_python=no
		else
			python_includes="-I $python_includes"
			python_libraries="-L $python_libraries -lpython$ver"
			usever=$ver
			enable_python=yes
		fi
	done

	if test "x$usever" = "x"; then
		AC_MSG_RESULT(no.)
	else
		AC_MSG_RESULT([ok, version $usever.])
		AC_SUBST(python_includes)
		AC_SUBST(python_libraries)
		AC_DEFINE([EMBED_PYTHON], 1, [Define to use embedded Python])
	fi
fi

dnl Create files
dnl ============

if test "x$prefix" = "xNONE"; then
	prefix="/usr/local"
fi

grubbydir="${prefix}/share/grubby"
grubbymoddir="${prefix}/lib/grubby"

AC_DEFINE_UNQUOTED([GRUBBYDIR], "${grubbydir}", [Grubby's directory])
AC_DEFINE_UNQUOTED([PREFIX], "${prefix}", [The installation prefix])

AC_SUBST(grubbymoddir)
AC_SUBST(grubbydir)
AC_SUBST(SUBDIRS)

AC_CONFIG_FILES([Makefile
		grubby/Makefile
		grubby/src/Makefile
		grubby/modules/Makefile
		grubby/data/Makefile
		grubby/data/grubby-config
		grubby/po/Makefile
		games/Makefile
		games/lib/Makefile
		games/guru-ttt/Makefile
		games/guru-ttt/module.dsc
		games/guru-chess/Makefile
		games/guru-chess/module.dsc
		man/Makefile
		man/de/Makefile])
AC_OUTPUT

dnl Status output
dnl =============

echo ""
echo "Configured grubby. Type 'make' to compile it, followed by a 'make install'."
echo ""
echo "Python support: $enable_python"
echo "Ruby support: $enable_ruby"
echo "Perl support: $enable_perl"
echo "Game engines: $enable_games"
echo ""

