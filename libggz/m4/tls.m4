dnl ======================================
dnl GGZ Gaming Zone - Configuration Macros
dnl ======================================
dnl
dnl Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
dnl This file has heavily been inspired by KDE's acinclude :)
dnl It is published under the conditions of the GNU General Public License.
dnl
dnl ======================================
dnl
dnl This file contains all autoconf macros needed for any security
dnl implementation such as TLS/SSL.
dnl
dnl ======================================
dnl
dnl History:
dnl 2002-02-10: lookup TLS libraries; taken code from acinclude.ggz
dnl 2002-02-24: default to GnuTLS; create conditional compile vars

dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_TLS - find a TLS implementation (support for gnutls, openssl and
dnl              none)
dnl AC_PATH_SSL - OpenSSL implementation backend (code from kdelibs)
dnl AC_GGZ_GNUTLS - GNUTLS implementation backend
dnl

dnl ------------------------------------------------------------------------
dnl Find a single file
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN(AC_GGZ_FIND_FILE,
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
dnl Try to find the SSL headers and libraries.
dnl $(SSL_LDFLAGS) will be -Lsslliblocation (if needed)
dnl and $(SSL_INCLUDES) will be -Isslhdrlocation (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN(AC_PATH_SSL,
[
LIBSSL="-lssl -lcrypto"

ac_ssl_includes=NO ac_ssl_libraries=NO
ssl_libraries=""
ssl_includes=""
AC_ARG_WITH(ssl-dir,
    [  --with-ssl-dir=DIR      where the root of OpenSSL is installed],
    [  ac_ssl_includes="$withval"/include
       ac_ssl_libraries="$withval"/lib
    ])

dnl AC_MSG_CHECKING(for OpenSSL)

AC_CACHE_VAL(ac_cv_have_ssl,
[#try to guess OpenSSL locations
  
  ssl_incdirs="/usr/include /usr/local/include /usr/ssl/include /usr/local/ssl/include $prefix/include $kde_extra_includes"
  ssl_incdirs="$ac_ssl_includes $ssl_incdirs"
  AC_GGZ_FIND_FILE(openssl/ssl.h, $ssl_incdirs, ssl_incdir)
  ac_ssl_includes="$ssl_incdir"

  ssl_libdirs="/usr/lib /usr/local/lib /usr/ssl/lib /usr/local/ssl/lib $prefix/lib $exec_prefix/lib $kde_extra_libs"
  if test ! "$ac_ssl_libraries" = "NO"; then
    ssl_libdirs="$ac_ssl_libraries $ssl_libdirs"
  fi

  test=NONE
  ssl_libdir=NONE
  for dir in $ssl_libdirs; do
    try="ls -1 $dir/libssl*"
    if test=`eval $try 2> /dev/null`; then ssl_libdir=$dir; break; else echo "tried $dir" >&AC_FD_CC ; fi
  done

  ac_ssl_libraries="$ssl_libdir"

  AC_LANG_SAVE
  AC_LANG_C

  ac_cflags_safe="$CFLAGS"
  ac_ldflags_safe="$LDFLAGS"
  ac_libs_safe="$LIBS"

  CFLAGS="$CFLAGS -I$ssl_incdir $all_includes"
  LDFLAGS="-L$ssl_libdir $all_libraries"
  LIBS="$LIBS $LIBSSL -lRSAglue -lrsaref"

  AC_TRY_LINK(,void RSAPrivateEncrypt(void);RSAPrivateEncrypt();,
  ac_ssl_rsaref="yes"
  ,
  ac_ssl_rsaref="no"
  )

  CFLAGS="$ac_cflags_safe"
  LDFLAGS="$ac_ldflags_safe"
  LIBS="$ac_libs_safe"

  AC_LANG_RESTORE

  if test "$ac_ssl_includes" = NO || test "$ac_ssl_libraries" = NO; then
    have_ssl=no
  else
    have_ssl=yes;
  fi

])

eval "$ac_cv_have_ssl"

dnl AC_MSG_RESULT([libraries $ac_ssl_libraries, headers $ac_ssl_includes])

dnl AC_MSG_CHECKING([whether OpenSSL uses rsaref])
dnl AC_MSG_RESULT($ac_ssl_rsaref)

dnl AC_MSG_CHECKING([for easter eggs])
dnl AC_MSG_RESULT([none found])

if test "$have_ssl" = yes; then
  dnl AC_MSG_CHECKING(for OpenSSL version)
  dnl Check for SSL version
  AC_CACHE_VAL(ac_cv_ssl_version,
  [
    AC_LANG_SAVE
    AC_LANG_C 

    cat >conftest.$ac_ext <<EOF
#include <openssl/opensslv.h>
#include <stdio.h>
    int main() {
 
#ifndef OPENSSL_VERSION_NUMBER
      printf("ssl_version=\\"error\\"\n");
#else
      if (OPENSSL_VERSION_NUMBER < 0x00906000)
        printf("ssl_version=\\"old\\"\n");
      else
        printf("ssl_version=\\"ok\\"\n");
#endif
     return (0);
    }
EOF

    ac_compile='${CC-gcc} $CFLAGS -I$ac_ssl_includes conftest.$ac_ext -o conftest'
    if AC_TRY_EVAL(ac_compile); then 

      if eval `./conftest 2>&5`; then
        if test $ssl_version = error; then
          dnl AC_MSG_WARN([$ssl_incdir/openssl/opensslv.h doesn't define OPENSSL_VERSION_NUMBER !])
		  have_ssl=no
        else
          if test $ssl_version = old; then
            dnl AC_MSG_WARN([OpenSSL version too old. Upgrade to 0.9.6 at least, see http://www.openssl.org. SSL support disabled.])
            have_ssl=no
          fi
        fi
        ac_cv_ssl_version="ssl_version=$ssl_version"
      else
        dnl AC_MSG_WARN([Your system couldn't run a small SSL test program.
        dnl Check config.log, and if you can't figure it out, send a mail to 
        dnl the GGZ development list <ggz-dev@lists.sourceforge.net>, attaching your config.log])
		have_ssl=no
      fi

    else
      dnl AC_MSG_WARN([Your system couldn't link a small SSL test program.
      dnl Check config.log, and if you can't figure it out, send a mail to 
      dnl the GGZ development list <ggz-dev@lists.sourceforge.net>, attaching your config.log])
	  have_ssl=no
    fi 

    AC_LANG_RESTORE

  ])

  eval "$ac_cv_ssl_version"
  dnl AC_MSG_RESULT($ssl_version)
fi

if test "$have_ssl" != yes; then
  LIBSSL="";
else
  AC_DEFINE(HAVE_SSL, 1, [If we are going to use OpenSSL])
  ac_cv_have_ssl="have_ssl=yes \
    ac_ssl_includes=$ac_ssl_includes ac_ssl_libraries=$ac_ssl_libraries ac_ssl_rsaref=$ac_ssl_rsaref"
  
  
  ssl_libraries="$ac_ssl_libraries"
  ssl_includes="$ac_ssl_includes"

  if test "$ac_ssl_rsaref" = yes; then
    LIBSSL="-lssl -lcrypto -lRSAglue -lrsaref" 
  fi

  if test $ssl_version = "old"; then
    AC_DEFINE(HAVE_OLD_SSL_API, 1, [Define if you have OpenSSL < 0.9.6])
  fi
fi

dnl if test "$ssl_includes" = "/usr/include" || test  "$ssl_includes" = "/usr/local/include" || test -z "$ssl_includes"; then
dnl SSL_INCLUDES="";
dnl else
 SSL_INCLUDES="-I $ssl_includes"
dnl fi

dnl if test "$ssl_libraries" = "/usr/lib" || test "$ssl_libraries" = "/usr/local/lib" || test -z "$ssl_libraries"; then
dnl  SSL_LDFLAGS=""
dnl else
 SSL_LDFLAGS="$ssl_libraries -R$ssl_libraries"
dnl fi

AC_SUBST(SSL_INCLUDES)
AC_SUBST(SSL_LDFLAGS)
AC_SUBST(LIBSSL)
])

dnl ------------------------------------------------------------------------
dnl Try to find the GNUTLS headers and libraries.
dnl $(GNUTLS_LDFLAGS) will be -L ... (if needed)
dnl and $(GNUTLS_INCLUDES) will be -I ... (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN(AC_GGZ_GNUTLS,
[
dnl AC_MSG_CHECKING([for GNUTLS])

ac_gnutls_includes=NO ac_gnutls_libraries=NO
gnutls_libraries=""
gnutls_includes=""

AC_ARG_WITH(gnutls-dir,
    [  --with-gnutls-dir=DIR       gnutls installation prefix ],
    [  ac_gnutls_includes="$withval"/include/gnutls
       ac_gnutls_libraries="$withval"/lib
    ])
AC_ARG_WITH(gnutls-includes,
    [  --with-gnutls-includes=DIR  where the gnutls includes are. ],
    [  ac_gnutls_includes="$withval"
    ])
AC_ARG_WITH(gnutls-libraries,
    [  --with-gnutls-libraries=DIR where the gnutls libs are. ],
    [  ac_gnutls_libraries="$withval"
    ])

AC_CACHE_VAL(ac_cv_have_gnutls,
[
if test "x${prefix}" = "xNONE"; then
   prefix_incdir="${ac_default_prefix}/include"
   prefix_libdir="${ac_default_prefix}/lib"
else
   prefix_incdir="${prefix}/include"
   prefix_libdir="${prefix}/lib"
fi
gnutls_incdirs="$ac_gnutls_includes $prefix_incdir/gnutls /usr/local/include/gnutls /usr/include/gnutls"
gnutls_header=gnutls.h

AC_GGZ_FIND_FILE($gnutls_header, $gnutls_incdirs, gnutls_incdir)
ac_gnutls_includes="$gnutls_incdir"

gnutls_libdirs="$ac_gnutls_libraries $prefix_libdir /usr/local/lib /usr/lib"

test=NONE
gnutls_libdir=NONE
for dir in $gnutls_libdirs; do
  try="ls -1 $dir/libgnutls.*"
  if test -n "`$try 2> /dev/null`"; then gnutls_libdir=$dir; break; else echo "tried $dir" >&AC_FD_CC ; fi
done

ac_gnutls_libraries="$gnutls_libdir"

if test "$ac_gnutls_includes" = NO || test "$ac_gnutls_libraries" = NO; then
  ac_cv_have_gnutls="have_gnutls=no"
  ac_gnutls_notfound=""

  dnl AC_MSG_ERROR([gnutls not found. Please check your installation! ]);
else
  have_gnutls="yes"
fi
])

eval "$ac_cv_have_gnutls"

if test "$have_gnutls" != yes; then
  dnl AC_MSG_RESULT([$have_gnutls]);
  have_gnutls=no
else
  ac_cv_have_gnutls="have_gnutls=yes \
    ac_gnutls_includes=$ac_gnutls_includes ac_gnutls_libraries=$ac_gnutls_libraries"
  dnl AC_MSG_RESULT([libraries $ac_gnutls_libraries, headers $ac_gnutls_includes])

  gnutls_libraries="$ac_gnutls_libraries"
  gnutls_includes="$ac_gnutls_includes"
fi

AC_SUBST(gnutls_libraries)
AC_SUBST(gnutls_includes)

GNUTLS_INCLUDES="-I$gnutls_includes"
GNUTLS_LDFLAGS="-L$gnutls_libraries -lgnutls -lgcrypt"

AC_SUBST(GNUTLS_INCLUDES)
AC_SUBST(GNUTLS_LDFLAGS)

LIB_GGZ='-lggz'
AC_SUBST(LIB_GGZ)

])

dnl ------------------------------------------------------------------------
dnl Try to find a suitable TLS implementation.
dnl $(GGZTLS_LDFLAGS) will be -L ... (if needed)
dnl and $(GGZTLS_INCLUDES) will be -I ... (if needed)
dnl $(GGZTLS_SOURCES) is the dynamic list of source files, depending on the
dnl used implementation
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN(AC_GGZ_TLS,
[
AC_MSG_CHECKING([for GGZ TLS implementation])

AC_ARG_WITH(tls,
     [  --with-tls[=ARG]        GnuTLS or OpenSSL - Auto if no ARG],
     tls_type=$withval, tls_type=no)

dnl None (defaults)
GGZTLS_INCLUDES=""
GGZTLS_LDFLAGS=""
LIB_GGZTLS=""
GGZTLS_SOURCES="none.c"
TLS_TYPE="no"

if test "$tls_type" = yes -o "$tls_type" = GnuTLS; then
  dnl GNUTLS check
  AC_GGZ_GNUTLS
  if test "$have_gnutls" = yes; then
    GGZTLS_INCLUDES=$GNUTLS_INCLUDES
    GGZTLS_LDFLAGS=$GNUTLS_LDFLAGS
    GGZTLS_SOURCES="gnutls.c"
    AC_MSG_RESULT([using GnuTLS])
    AC_DEFINE_UNQUOTED(GGZ_TLS_GNUTLS)
    TLS_TYPE="GnuTLS"
  fi
fi

if test \( "$tls_type" = yes -a "$have_gnutls" = no \) -o "$tls_type" = OpenSSL
then
  dnl OpenSSL check
  AC_PATH_SSL
  if test "$have_ssl" = yes; then
    GGZTLS_INCLUDES=$SSL_INCLUDES
    GGZTLS_LDFLAGS=$LIBSSL
    GGZTLS_SOURCES="openssl.c list.c list.h"
    AC_MSG_RESULT([using OpenSSL])
    AC_DEFINE_UNQUOTED(GGZ_TLS_OPENSSL)
    TLS_TYPE="OpenSSL"
  fi
fi

if test "$TLS_TYPE" = no; then
  if test "$tls_type" = no; then
    AC_MSG_RESULT([no])
  else
    AC_MSG_WARN([No TLS implementation found - using 'no'!])
  fi
  AC_DEFINE_UNQUOTED(GGZ_TLS_NONE)
fi


AC_SUBST(GGZTLS_INCLUDES)
AC_SUBST(GGZTLS_LDFLAGS)
AC_SUBST(LIB_GGZTLS)
AC_SUBST(GGZTLS_SOURCES)
AC_SUBST(GGZTLS_LIB)

AM_CONDITIONAL(TLS_NONE, test x$TLS_TYPE = xno)
AM_CONDITIONAL(TLS_OPENSSL, test x$TLS_TYPE = xOpenSSL)
AM_CONDITIONAL(TLS_GNUTLS, test x$TLS_TYPE = xGnuTLS)

])

