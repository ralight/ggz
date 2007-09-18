#!/bin/sh
#
# Shows differences between GGZ 0.0.14 branch and GNOME SVN

gnomedepsurl=http://svn.gnome.org/svn/gnome-games/trunk/dependencies
branchurl=svn://svn.ggzgamingzone.org/svn/branches/ggz-0-0-14

#diffdir=/tmp/diff-to-gnome-$$
diffdir=/tmp/diff-to-gnome-stow

mkdir -p $diffdir
cd $diffdir
echo "Producing diffs in $diffdir..."

svn co $branchurl/libggz/src libggz.ggz
svn co $gnomedepsurl/libggz libggz.gnome
diff --exclude=.svn -Nur libggz.ggz libggz.gnome > libggz.diff

svn co $branchurl/ggz-client-libs/ggzmod ggzmod.ggz
svn co $gnomedepsurl/ggzmod ggzmod.gnome
diff --exclude=.svn -Nur ggzmod.ggz ggzmod.gnome > ggzmod.diff

svn co $branchurl/ggz-client-libs/ggzmod-ggz ggzmod-ggz.ggz
svn co $gnomedepsurl/ggzmod-ggz ggzmod-ggz.gnome
diff --exclude=.svn -Nur ggzmod-ggz.ggz ggzmod-ggz.gnome > ggzmod-ggz.diff

svn co $branchurl/ggz-client-libs/ggzmod ggzmod.ggz
svn co $gnomedepsurl/ggzmod ggzmod.gnome
diff --exclude=.svn -Nur ggzmod.ggz ggzmod.gnome > ggzmod.diff

svn co $branchurl/ggz-client-libs/ggzcore ggzcore.ggz
svn co $gnomedepsurl/ggzcore ggzcore.gnome
diff --exclude=.svn -Nur ggzcore.ggz ggzcore.gnome > ggzcore.diff

svn co $branchurl/ggzd/ggzdmod ggzdmod.ggz
svn co $gnomedepsurl/ggzdmod ggzdmod.gnome
diff --exclude=.svn -Nur ggzdmod.ggz ggzdmod.gnome > ggzdmod.diff

echo "Produced diffs in $diffdir!"

#rm -rf $diffdir

