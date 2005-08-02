#!/bin/bash
#
# Bootstrap build for KGGZ
#
# This isn't the actual makefile, but a nice replacement
# for a real bootstrap or autogen.sh. Everything is built in "buildtree".
#
# (C) 2001 Josef Spillner
# dr_maux@users.sourceforge.net
# The MindX Open Source Project
# http://mindx.sourceforge.net/support/ggz/

echo "KGGZ Installation"
#echo "Program output goes to logfile.txt"
echo "* bootstrapping..."
rm -rf buildtree
mkdir buildtree
cp -r data src ggzcore++ buildtree/
cp admin/* buildtree
cd buildtree
#libtoolize --force --copy 1>logfile.txt
libtoolize --force --copy
aclocal
autoheader
#automake --gnu --add-missing --include-deps 2>>logfile.txt
automake --gnu --add-missing --include-deps
autoconf
perl am_edit
echo "* configuring..."
#./configure 1>>logfile.txt 2>>logfile.txt
./configure
echo "* compiling..."
echo "Please cd into the buildtree/ directory and type: 'make'"
#make >>logfile.txt
#make
#echo "* installing as root..."
#su --command="make install >>logfile.txt"
#su --command="make install"
#echo "* ready!"
#echo "KGGZ installation successful."
