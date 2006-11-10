#!/bin/sh

id=`id -u`
if test $id != 0; then
  echo "** Warning: Setup script must be run as root!"
  echo "** Proceeding might result in incomplete integration."
  echo ""
  #exit 1
fi

# Note: the configuration part doesn't reall need root privileges but it's simpler that way

configfile=$1
if [ -z $configfile ]; then
  configfile=~/.ggzcommunity.setup
fi

if [ ! -f $configfile ]; then
  echo "** Error: No configuration found in $configfile!"
  echo "** Call 'make setup' first."
  exit 1
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

echo "Sanity checks..."
if [ -d $documentroot ]; then
  rm -rf $documentroot
fi
if [ ! -d $ggzdir ]; then
  echo "Error: GGZ directory $ggzdir does not exist."
  exit 1
fi
echo "Web pages..."
cp -r ../web $documentroot
echo "Web server..."
cp webserver/ggz-apache2.conf $vhostdir/ggzcommunity
a2ensite ggzcommunity
if test "x$dblocal" = "xy"; then
  echo "Database setup; needs the root/sudo password."
  sudo su -c "createuser -A -R -D -P $dbuser" postgres || echo "User $dbuser exists already? Error during creation."
  sudo su -c "createdb -O $dbuser $dbname" postgres || echo "Database $dbname exists already? Error during creation."
fi
if test "x$phpbbdblocal" = "xy"; then
  echo "phpBB database setup; needs the root/sudo password."
  sudo su -c "createuser -A -R -D -P $phpbbdbuser" postgres || echo "User $phpbbdbuser exists already? Error during creation."
  sudo su -c "createdb -O $phpbbdbuser $phpbbdbname" postgres || echo "Database $phpbbdbname exists already? Error during creation."
fi
#echo "Integration of external software..."
#cp scripts/ggz2phpbb.conf $ggzdir/etc/ggzd
#(cd external && tar xzf l10n-data.tar.gz)
#cp -r external/l10n-data $documentroot/db/ggzicons/flags
#(cd external && tar xzf hotstuff-*.tar.gz)
#cp -r external/hotstuff*/ $documentroot/hotstuff/directory
#(cd external && tar xjf phpBB-*.tar.bz2)
#cp -r external/phpBB*/ $documentroot/forums/phpBB2
#(cd external && tar xjf planet-*.tar.bz2)
#cp -r external/planet*/ $documentroot/blogs
echo "Scripts and tools..."
(cd scripts/calcrankings && make install)
cp scripts/ggz2phpbb.pl $ggzdir/bin/ggz2phpbb
cp scripts/ggzblogs.pl $ggzdir/bin/ggzblogs
echo "Site modifications..."
mkdir -p $documentroot/blogs/config
cp blogs/*.* $documentroot/blogs/config

