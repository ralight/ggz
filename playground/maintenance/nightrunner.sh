#!/bin/sh

# Update local SVN checkout (SVN trunk)
cd ~
svn co svn://svn.ggzgamingzone.org/svn/trunk localcheckout

# Keep WWW pages (www.g.o, dev.g.o) current
cd /var/www/ggz
svn co svn://svn.ggzgamingzone.org/svn/trunk/www www

# Keep generated documentation (www.g.o/docs) current
cd ~/localcheckout
~/localcheckout/playground/maintenance/ggzbuild ../ggzbuildrc.nightly
~/findmissingdocs.sh

