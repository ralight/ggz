#!/bin/sh
# GGZ Gaming Zone bootstrap script
# To be used for CVS checkouts

srcdir=`dirname $0`
DIE=0

if test -z "$*"; then
  echo "** Warning: I am going to run 'configure' with no arguments."
  echo "** If you wish to pass any to it, please specify them on the"
  echo "** '$0' command line."
  echo
fi

echo -n "Bootstrapping GGZ... "
echo -n "[prerequisites]"

# Make sure autoconf is installed
(autoconf --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`autoconf' installed to."
  echo "Download the appropriate package for your distribution,"
  echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
  DIE=1
}

# Make sure libtool is installed
(grep "^AM_PROG_LIBTOOL" $srcdir/configure.in >/dev/null) && {
  (libtool --version) < /dev/null > /dev/null 2>&1 && need_libtool=1 || {
    echo
    echo "**Error**: You must have \`libtool' installed."
    echo "Get ftp://ftp.gnu.org/pub/gnu/libtool-1.4.tar.gz"
    echo "(or a newer version if it is available)"
    DIE=1
  }
}

# Make sure automake is installed
(automake --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`automake' installed."
  echo "Get ftp://ftp.gnu.org/pub/gnu/automake-1.5.tar.gz"
  echo "(or a newer version if it is available)"
  DIE=1
  NO_AUTOMAKE=yes
}


# if no automake, don't bother testing for aclocal
test -n "$NO_AUTOMAKE" || (aclocal --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: Missing \`aclocal'.  The version of \`automake'"
  echo "installed doesn't appear recent enough."
  echo "Get ftp://ftp.gnu.org/pub/gnu/automake-1.5.tar.gz"
  echo "(or a newer version if it is available)"
  DIE=1
}

if test "$DIE" -eq 1; then
  exit 1
fi


echo -n "[m4]"
rm -f $srcdir/acinclude.m4
if test -f $srcdir/acinclude.ggz; then
	cat $srcdir/acinclude.ggz >> $srcdir/acinclude.m4;
fi
if test -f $srcdir/acinclude.kde; then
	cat $srcdir/acinclude.kde >> $srcdir/acinclude.m4;
fi
if test "x$need_libtool" = "x1"; then
	echo -n "[libtoolize]"
	(cd $srcdir && libtoolize --force --copy) || { echo "libtoolize failed."; exit; }
fi
echo -n "[aclocal]"
(cd $srcdir && aclocal) || { echo "aclocal failed."; exit; }
echo -n "[autoheader]"
autoheader -I $srcdir || { echo "autoheader failed."; exit; }
echo -n "[automake]"
(cd $srcdir && automake --add-missing --gnu) || { echo "automake failed."; exit; }
echo -n "[autoconf]"
autoconf -I $srcdir $srcdir/configure.in > $srcdir/configure && chmod +x $srcdir/configure || { echo "autoconf failed."; exit; }
echo ""

#conf_flags="--enable-maintainer-mode --enable-compile-warnings" #--enable-iso-c

if test x$NOCONFIGURE = x; then
  echo Running $srcdir/configure $conf_flags "$@" ...
  $srcdir/configure $conf_flags "$@"
else
  echo Skipping configure process.
fi

