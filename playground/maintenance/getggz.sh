#!/bin/sh
# Performs a local GGZ installation
# Copyright (C) 2004 - 2007 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

version=0.0.14
tmp=$HOME/.ggz/getggz-$version
log=$tmp/getggz.log
prefix=$HOME/ggz
makeprog=make
cache=n

echo "-----------------------"
echo "GGZ $version Installation"
echo "-----------------------"

if test -f /usr/bin/gmake || test -f /usr/pkg/bin/gmake; then
	echo "*** Note: Detected BSD layout"
	makeprog=gmake
fi
echo "*** Note: Log file will be written to $log"

echo "Please select a download mirror:"
echo "USA, via ibiblio           (1)"
echo "Denmark, via dotsrc        (2)"
echo "Belgium, via Belnet        (3)"
read -p "? " mirror

case $mirror in
	1)
	mirror="ftp://ftp.ibiblio.org/pub/mirrors/ggzgamingzone"
	;;
	2)
	mirror="ftp://ftp.dotsrc.org/mirrors/ggzgamingzone"
	;;
	3)
	mirror="ftp://ftp.belnet.be/mirrors/ftp.ggzgamingzone.org"
	;;
	*)
	echo "Invalid mirror!"
	exit
	;;
esac

echo "Please select the installation method:"
echo "Client (c)"
echo "Server (s)"
read -p "? " method

case $method in
	c)
	pkg="libggz ggz-client-libs"
	;;
	s)
	pkg="libggz ggz-server ggz-client-libs ggz-utils ggz-grubby"
	;;
	*)
	echo "Invalid method!"
	exit
	;;
esac

if test "x$method" = "xc"; then
	echo "Include KDE packages (y/n)?"
	read -p "? " kde
	case $kde in
		y)
		pkg="$pkg ggz-kde-games ggz-kde-client"
		;;
		n)
		;;
		*)
		echo "Invalid answer!"
		exit
		;;
	esac

	echo "Include Gtk+ packages (y/n)?"
	read -p "? " gtk
	case $gtk in
		y)
		pkg="$pkg ggz-gtk-games ggz-gtk-client"
		;;
		n)
		;;
		*)
		echo "Invalid answer!"
		exit
		;;
	esac

	if test "x$gtk" = "xy"; then
		echo "Even include GNOME packages (y/n)?"
		read -p "? " gnome
		case $gnome in
			y)
			pkg="$pkg ggz-gnome-client"
			;;
			n)
			;;
			*)
			echo "Invalid answer!"
			exit
			;;
		esac
	fi

	echo "Include Console/ncurses packages (y/n)?"
	read -p "? " txt
	case $txt in
		y)
		pkg="$pkg ggz-txt-client"
		;;
		n)
		;;
		*)
		echo "Invalid answer!"
		exit
		;;
	esac

	echo "Include SDL packages (y/n)?"
	read -p "? " sdl
	case $sdl in
		y)
		pkg="$pkg ggz-sdl-games"
		;;
		n)
		;;
		*)
		echo "Invalid answer!"
		exit
		;;
	esac

	if test "x$sdl" = "xy"; then
		echo "Even include SDL/Python/pygame packages (y/n)?"
		read -p "? " python
		case $python in
			y)
			pkg="$pkg ggz-python"
			;;
			n)
			;;
			*)
			echo "Invalid answer!"
			exit
			;;
		esac
	fi
fi

if test -d $tmp; then
	echo "It appears getggz.sh was ran already."
	echo "Use cache to save time (y/n)?"
	read -p "? " cache
fi

echo "Downloading the following packages:"
echo $pkg
echo "From $mirror into $tmp"

mkdir -p $tmp
cd $tmp
rm -f $log

echo "Download of all packages"
for p in $pkg; do
	tarball=$mirror/ggz/$version/$p-$version.tar.gz
	if test $cache = y && test -f $p-$version.tar.gz; then
		echo "- $p (download cached)"
		wget -c $tarball >>$log 2>&1
	else
		echo "+ $p (download)"
		wget $tarball >>$log 2>&1
	fi
	if test $? -ne 0; then
		echo "*** Downloading $tarball failed! (See $log)"
		exit
	fi
done

echo "Unpacking of all packages"
for p in $pkg; do
	if test $cache = y && test -d $p-$version; then
		echo "- $p (decompression cached)"
	else
		echo "+ $p (decompression)"
		tar xzvf $p-$version.tar.gz >>$log 2>&1
		if test $? -ne 0; then
			echo "*** Decompressing $p-$version.tar.gz failed! (See $log)"
			exit
		fi
	fi
done

echo "Configuration and compilation of all packages"
echo "Packages will be installed into $prefix"
export LD_LIBRARY_PATH=$prefix/lib:$LD_LIBRARY_PATH
for p in $pkg; do
	if test $cache = y && test -f $p-$version/.installed; then
		echo "- $p (configuration/compilation/installation cached)"
	else
		echo "+ $p (configuration)"
		cd $p-$version
		./configure --prefix=$prefix >>$log 2>&1
		if test $? -ne 0; then
			echo "*** Configuration of $p-$version failed! (See $log)"
			exit
		fi
		echo "+ $p (compilation)"
		$makeprog >>$log 2>&1
		if test $? -ne 0; then
			echo "*** Compilation of $p failed! (See $log)"
			exit
		fi
		echo "+ $p (installation)"
		$makeprog install >>$log 2>&1
		if test $? -ne 0; then
			echo "*** Installation of $p failed! (See $log)"
			exit
		fi
		touch .installed
		cd $tmp
	fi
done

echo "Finished! GGZ $version is now installed."
echo "Do not forget to set up the following paths:"
echo "export PATH=$prefix/bin:\$PATH"
echo "export LD_LIBRARY_PATH=$prefix/lib:\$LD_LIBRARY_PATH"

if test "x$method" = "xc"; then
	launcher=$prefix/bin/launchggzclient.sh
else
	launcher=$prefix/bin/launchggzserver.sh
fi
:> $launcher
echo "export PATH=$prefix/bin:\$PATH" >> $launcher
echo "export LD_LIBRARY_PATH=$prefix/lib:\$LD_LIBRARY_PATH" >> $launcher
if test "x$method" = "xc"; then
	echo "ggz" >> $launcher
else
	echo "ggzd -F" >> $launcher
fi
chmod +x $launcher

echo ""
echo "Or simply run the following script: $launcher"

