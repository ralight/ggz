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

# Make sure autotools are installed

version_check()
{
log="$log\t++ Checking for $1,"
($1 --version >/dev/null 2>&1) || { log="$log not found!\n"; bailout=1; }
vline=`$1 --version | head -1`
needed="$2"

version=`echo $vline | sed 's/^[A-z-\.\ ()]*//;s/ .*$//'`
vmajor="0`echo $version | cut -d . -f 1`"
vminor="0`echo $version | cut -s -d . -f 2`"
vmicro="0`echo $version | cut -s -d . -f 3`"
nmajor="0`echo $needed | cut -d . -f 1`"
nminor="0`echo $needed | cut -s -d . -f 2`"
nmicro="0`echo $needed | cut -s -d . -f 3`"

old=0
if [ $vmajor -lt $nmajor ]; then old=1;
elif [ $vminor -lt $nminor ]; then old=1;
elif [ $vmicro -lt $nmicro ]; then old=1;
fi

vversion=`echo "$vmajor.$vminor.$vmicro" | perl -e "\\$_ = <STDIN>; \\$_ =~ s/(^|\.)0(\d+)/\\$1\\$2/g; print"`
nversion=`echo "$nmajor.$nminor.$nmicro" | perl -e "\\$_ = <STDIN>; \\$_ =~ s/(^|\.)0(\d+)/\\$1\\$2/g; print"`

log="$log need $nversion, found $vversion -"
if test $old = "1"; then
  log="$log too old!\n"
  bailout=1
else
  log="$log ok.\n"
fi
}

log=""
bailout=0

version_check "autoconf" "2.5"
version_check "automake" "1.5"
version_check "libtool" "1.4.3"
version_check "gettext" "0.11"

if test "x$bailout" = "x1"; then
  echo
  echo "** Error: some prerequisites could not be found:"
  echo -e $log
  exit
fi

if test "$DIE" -eq 1; then
  exit 1
fi

# Run autotools suite

if test -d $srcdir/m4; then
	echo -n "[m4]"
	cat $srcdir/m4/*.m4 > $srcdir/acinclude.m4
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
if test -f $srcdir/am_edit; then
	echo -n "[am_edit]"
	perl $srcdir/am_edit --foreign-libtool || { echo "am_edit failed."; exit; }
fi
echo -n "[autoconf]"
autoconf -I $srcdir $srcdir/configure.ac > $srcdir/configure && chmod +x $srcdir/configure || { echo "autoconf failed."; exit; }
echo ""

# Run configuration

if test x$NOCONFIGURE = x; then
  echo Running $srcdir/configure $conf_flags "$@" ...
  $srcdir/configure $conf_flags "$@"
else
  echo Skipping configure process.
fi

