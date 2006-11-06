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

hostname=`grep hostname $configfile 2>/dev/null | cut -d "=" -f 2`
documentroot=`grep documentroot $configfile 2>/dev/null | cut -d "=" -f 2`
dblocal=`grep dblocal $configfile 2>/dev/null | cut -d "=" -f 2`
dbhost=`grep dbhost $configfile 2>/dev/null | cut -d "=" -f 2`
dbname=`grep dbname $configfile 2>/dev/null | cut -d "=" -f 2`
dbuser=`grep dbuser $configfile 2>/dev/null | cut -d "=" -f 2`
dbpass=`grep dbpass $configfile 2>/dev/null | cut -d "=" -f 2`

[ -z $documentroot ] && documentroot=/var/www/ggzcommunity
[ -z $dblocal ] && dblocal=y
[ -z $vhostdir ] && vhostdir=/etc/apache2/sites-available

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
echo "3.2 Database password"
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
echo "Confirmation"
echo "Hostname.....$hostname"
echo "Directory....$documentroot"
if test $dblocal = "n"; then
  echo "Database.....$dbuser@$dbhost/$dbname (no local setup)"
else
  echo "Database.....$dbuser@$dbhost/$dbname (configure locally)"
fi
echo "Web server...$vhostdir"

echo ""
echo "Creating configuration files..."
hostname_quoted=`echo $hostname | sed -e "s/\//\\\\\\\\\//g"`
documentroot_quoted=`echo $documentroot | sed -e "s/\//\\\\\\\\\//g"`
cp webserver/ggz-apache2.conf.in webserver/ggz-apache2.conf
sed -i -e "s/\@SERVERNAME\@/$hostname_quoted/" webserver/ggz-apache2.conf
sed -i -e "s/\@DOCUMENTROOT\@/$documentroot_quoted/" webserver/ggz-apache2.conf

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
    echo "Database setup; remember the database password for the user ('role')."
    su -c "echo $dbpass | createuser -A -R -D -P $dbuser" postgres || echo "User $dbuser exists already? Error during creation."
    su -c "createdb -O $dbuser $dbname" postgres || echo "Database $dbname exists already? Error during creation."
  fi
fi

