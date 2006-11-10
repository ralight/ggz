#!/bin/sh

configfile=$1
if [ -z $configfile ]; then
  configfile=~/.ggzcommunity.setup
fi

hostname=`grep ^hostname $configfile 2>/dev/null | cut -d "=" -f 2`
documentroot=`grep ^documentroot $configfile 2>/dev/null | cut -d "=" -f 2`
dblocal=`grep ^dblocal $configfile 2>/dev/null | cut -d "=" -f 2`
dbhost=`grep ^dbhost $configfile 2>/dev/null | cut -d "=" -f 2`
dbname=`grep ^dbname $configfile 2>/dev/null | cut -d "=" -f 2`
dbuser=`grep ^dbuser $configfile 2>/dev/null | cut -d "=" -f 2`
dbpass=`grep ^dbpass $configfile 2>/dev/null | cut -d "=" -f 2`
vhostdir=`grep ^vhostdir $configfile 2>/dev/null | cut -d "=" -f 2`
phpbbdblocal=`grep ^phpbbdblocal $configfile 2>/dev/null | cut -d "=" -f 2`
phpbbdbhost=`grep ^phpbbdbhost $configfile 2>/dev/null | cut -d "=" -f 2`
phpbbdbname=`grep ^phpbbdbname $configfile 2>/dev/null | cut -d "=" -f 2`
phpbbdbuser=`grep ^phpbbdbuser $configfile 2>/dev/null | cut -d "=" -f 2`
phpbbdbpass=`grep ^phpbbdbpass $configfile 2>/dev/null | cut -d "=" -f 2`
ggzdir=`grep ^ggzdir $configfile 2>/dev/null | cut -d "=" -f 2`

[ -z $documentroot ] && documentroot=/var/www/ggzcommunity
[ -z $dblocal ] && dblocal=y
[ -z $phpbbdblocal ] && phpbbdblocal=y
[ -z $vhostdir ] && vhostdir=/etc/apache2/sites-available
[ -z $ggzdir ] && ggzdir=`ggz-config -c`/..

rm -f $configfile

echo "GGZ Community Interactive Setup"
echo "==============================="
echo ""
echo "This script is going to ask a few questions."
echo "Afterwards, GGZ Community will be installed and configured accordingly."
echo ""
echo "1. Hostname"
echo "A virtual host entry is required for GGZ Community. Please make sure that"
echo "this hostname resolves to an IP address on this machine."
read -p "Hostname [$hostname]: " xhostname
[ $xhostname ] && hostname=$xhostname
echo "hostname=$hostname" >> $configfile
echo ""
echo "2. Directory"
echo "The directory all of the web pages should be installed into."
read -p "Directory [$documentroot]: " xdocumentroot
[ $xdocumentroot ] && documentroot=$xdocumentroot
echo "documentroot=$documentroot" >> $configfile
echo ""
echo "3. Database"
echo "GGZ Community needs a database."
read -p "Setup local database (y/n) [$dblocal]? " xdblocal
if test "x$xdblocal" = "xy" || test "x$xdblocal" = "xn"; then
  dblocal=$xdblocal
fi
echo "dblocal=$dblocal" >> $configfile
echo ""
if test $dblocal = "y"; then
[ -z $dbhost ] && dbhost="localhost"
fi
echo "3.1 Database hostname"
echo "The hostname for the database."
read -p "Database hostname [$dbhost]? " xdbhost
[ $xdbhost ] && dbhost=$xdbhost
echo "dbhost=$dbhost" >> $configfile
echo ""
echo "3.2 Database name"
echo "The name for the database."
read -p "Database name [$dbname]? " xdbname
[ $xdbname ] && dbname=$xdbname
echo "dbname=$dbname" >> $configfile
echo ""
echo "3.3 Database user"
echo "The user name for the database."
read -p "Database user [$dbuser]? " xdbuser
[ $xdbuser ] && dbuser=$xdbuser
echo "dbuser=$dbuser" >> $configfile
echo ""
echo "3.4 Database password"
echo "The password for the user of the database."
read -p "Database pass [$dbpass]? " xdbpass
[ $xdbpass ] && dbpass=$xdbpass
echo "dbpass=$dbpass" >> $configfile
echo ""
echo "4. Web server"
echo "The location for adding virtual hosts."
read -p "Directory [$vhostdir]: " xvhostdir
[ $xvhostdir ] && vhostdir=$xvhostdir
echo "vhostdir=$vhostdir" >> $configfile
echo ""
echo "5. phpBB integration"
echo "The phpBB forum software can be integrated."
read -p "Setup local phpBB database (y/n) [$phpbbdblocal]? " xphpbbdblocal
if test "x$xphpbbdblocal" = "xy" || test "x$xphpbbdblocal" = "xn"; then
  phpbbdblocal=$xphpbbdblocal
fi
echo "phpbbdblocal=$phpbbdblocal" >> $configfile
echo ""
if test $phpbbdblocal = "y"; then
[ -z $phpbbdbhost ] && phpbbdbhost="localhost"
fi
echo "5.1 phpBB database hostname"
echo "The hostname for the database."
read -p "Database hostname [$phpbbdbhost]? " xphpbbdbhost
[ $xphpbbdbhost ] && phpbbdbhost=$xphpbbdbhost
echo "phpbbdbhost=$phpbbdbhost" >> $configfile
echo ""
echo "5.2 phpBB database name"
echo "The name for the database."
read -p "Database name [$phpbbdbname]? " xphpbbdbname
[ $xphpbbdbname ] && phpbbdbname=$xphpbbdbname
echo "phpbbdbname=$phpbbdbname" >> $configfile
echo ""
echo "5.3 phpBB database user"
echo "The user name for the database."
read -p "Database user [$phpbbdbuser]? " xphpbbdbuser
[ $xphpbbdbuser ] && phpbbdbuser=$xphpbbdbuser
echo "phpbbdbuser=$phpbbdbuser" >> $configfile
echo ""
echo "5.4 phpBB database password"
echo "The password for the user of the database."
read -p "Database pass [$phpbbdbpass]? " xphpbbdbpass
[ $xphpbbdbpass ] && phpbbdbpass=$xphpbbdbpass
echo "phpbbdbpass=$phpbbdbpass" >> $configfile
echo ""
echo "6. GGZ installation"
echo "The directory the GGZ server is installed in (prefix)."
read -p "GGZ directory [$ggzdir]: " xggzdir
[ $xggzdir ] && ggzdir=$xggzdir
echo "ggzdir=$ggzdir" >> $configfile
echo ""
echo "Confirmation"
echo "Hostname.....$hostname"
echo "Directory....$documentroot"
if test $dblocal = "n"; then
  echo "Database.....$dbuser@$dbhost/$dbname (no local setup)"
else
  echo "Database.....$dbuser@$dbhost/$dbname (configure locally)"
fi
echo "Web server...$vhostdir"
if test $phpbbdblocal = "n"; then
  echo "phpBB database.....$phpbbdbuser@$phpbbdbhost/$phpbbdbname (no local setup)"
else
  echo "phpBB database.....$phpbbdbuser@$phpbbdbhost/$phpbbdbname (configure locally)"
fi
echo "GGZ directory....$ggzdir"

./setup-build.sh

