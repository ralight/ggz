#!/bin/sh

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

#echo ""
#echo "Downloading external software..."
#(cd external && make fetch)

echo ""
read -p "Install now (y/n)? " install
if test "x$install" = "xy"; then
  ./setup-install.sh
fi

