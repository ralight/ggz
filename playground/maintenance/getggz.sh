#!/bin/sh
# Performs a local GGZ installation
# Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
# Published under GNU GPL conditions

version=0.0.9
tmp=$HOME/.ggz/getggz-$version
prefix=$HOME/ggz

echo "GGZ $version Installation"
echo "----------------------"

echo "Please select the installation method:"
echo "Client (c)"
echo "Server (s)"
read -p "? " method

case $method in
	c)
	pkg="libggz ggz-client-libs"
	;;
	s)
	pkg="libggz ggzd ggz-client-libs ggz-utils ggz-grubby"
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

echo "Downloading the following packages into $tmp:"
echo $pkg

mkdir -p $tmp
cd $tmp
for p in $pkg; do
	wget http://ftp.ggzgamingzone.org/pub/$version/$p-$version.tar.gz
done

echo "Unpacking of all packages"
for p in $pkg; do
	tar xzvf $p-$version.tar.gz
done

echo "Configuration and compilation of all packages"
for p in $pkg; do
	cd $p-$version
	./configure --prefix=$prefix
	make
	cd $tmp
done

echo "Installation into $prefix"
for p in $pkg; do
	cd $p-$version
	make install
	cd $tmp
done

echo "Finished!"

