#!/bin/sh

id=`id -u`
if test $id != 0; then
  echo "** Warning: Setup script must be run as root!"
  echo "** Proceeding might result in incomplete integration."
  echo ""
  #exit 1
fi

# Note: the configuration part doesn't reall need root privileges but it's simpler that way

configfile=~/.ggzcommunity.setup

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

echo ""
echo "Creating configuration files..."
hostname_quoted=`echo $hostname | sed -e "s/\//\\\\\\\\\//g"`
documentroot_quoted=`echo $documentroot | sed -e "s/\//\\\\\\\\\//g"`
phpbbdbhost_quoted=`echo $phpbbdbhost | sed -e "s/\//\\\\\\\\\//g"`
phpbbdbname_quoted=`echo $phpbbdbname | sed -e "s/\//\\\\\\\\\//g"`
phpbbdbuser_quoted=`echo $phpbbdbuser | sed -e "s/\//\\\\\\\\\//g"`
phpbbdbpass_quoted=`echo $phpbbdbpass | sed -e "s/\//\\\\\\\\\//g"`
cp webserver/ggz-apache2.conf.in webserver/ggz-apache2.conf
sed -i -e "s/\@SERVERNAME\@/$hostname_quoted/" webserver/ggz-apache2.conf
sed -i -e "s/\@DOCUMENTROOT\@/$documentroot_quoted/" webserver/ggz-apache2.conf
cp scripts/ggz2phpbb.conf.in scripts/ggz2phpbb.conf
sed -i -e "s/\@PHPBBHOST\@/$phpbbdbhost_quoted/" scripts/ggz2phpbb.conf
sed -i -e "s/\@PHPBBNAME\@/$phpbbdbname_quoted/" scripts/ggz2phpbb.conf
sed -i -e "s/\@PHPBBUSER\@/$phpbbdbuser_quoted/" scripts/ggz2phpbb.conf
sed -i -e "s/\@PHPBBPASS\@/$phpbbdbpass_quoted/" scripts/ggz2phpbb.conf

echo ""
echo "Compilation..."
(cd scripts/calcrankings && make)

echo ""
echo "Downloading external software..."
(cd external && make fetch)

echo ""
read -p "Install now (y/n)? " install
if test "x$install" = "xy"; then
  echo "Sanity checks..."
  if [ -d $documentroot ]; then
    rm -rf $documentroot
  fi
  echo "Web pages..."
  cp -r ../web $documentroot
  echo "Web server..."
  cp webserver/ggz-apache2.conf $vhostdir/ggzcommunity
  a2ensite ggzcommunity
  if test "x$dblocal" = "xy"; then
    echo "Database setup; needs the root/sudo password."
    su -c "su -c \"createuser -A -R -D -P $dbuser\" postgres" || echo "User $dbuser exists already? Error during creation."
    su -c "su -c \"createdb -O $dbuser $dbname\" postgres" || echo "Database $dbname exists already? Error during creation."
  fi
  if test "x$phpbbdblocal" = "xy"; then
    echo "phpBB database setup; needs the root/sudo password."
    su -c "su -c \"createuser -A -R -D -P $phpbbdbuser\" postgres" || echo "User $phpbbdbuser exists already? Error during creation."
    su -c "su -c \"createdb -O $phpbbdbuser $phpbbdbname\" postgres" || echo "Database $phpbbdbname exists already? Error during creation."
  fi
  echo "Integration of external software..."
  cp scripts/ggz2phpbb.conf $ggzdir/etc/ggzd
  (cd external && tar xzf l10n-data.tar.gz)
  cp -r external/l10n-data $documentroot/db/ggzicons/flags
  (cd external && tar xzf hotstuff-*.tar.gz)
  cp -r external/hotstuff*/ $documentroot/hotstuff/directory
  (cd external && tar xjf phpBB-*.tar.bz2)
  cp -r external/phpBB*/ $documentroot/forums/phpBB2
  (cd external && tar xjf planet-*.tar.bz2)
  cp -r external/planet*/ $documentroot/blogs
  echo "Scripts and tools..."
  (cd scripts/calcrankings && make install)
  cp scripts/ggz2phpbb.pl $ggzdir/bin/ggz2phpbb
  cp scripts/ggzblogs.pl $ggzdir/bin/ggzblogs
  echo "Site modifications..."
  cp blogs/*.* $documentroot/blogs/config
fi

