#!/bin/sh
#
# Checks for missing files in the mostly auto-generated documentation folder
# Must be able to access a SVN checkout of GGZ (see $svnroot) and an
# installation of ggz-docs (see $instroot)

svnroot=/home/ggznew/svn-version/
instroot=/home/ggznew/BUILD/
webroot=..

# No need to change anything from here on

docroot=$webroot/docs
sysdocroot=$instroot/share/doc/ggz-docs
origdir=$PWD

# Sanity checks

if [ ! -d $svnroot/docs ] || [ ! -d $svnroot/playground ]; then
	echo "Boy, $svnroot doesn't look like a SVN checkout to me!"
	exit 1
fi

if [ ! -d $sysdocroot/ggz-project ]; then
	echo "The ggz docs were certainly not installed into $instroot!"
	exit 1
fi

if [ ! -d $docroot ]; then
	echo "No GGZ webpages found in $webroot!"
	exit 1
fi

# Here be admin, developer and player docs as installed by ggz-docs

echo "Standard docs =>"

if [ ! -d $docroot/ggzdb ]; then
	echo " (install) ggzdb"
	cp -r $sysdocroot/server/database $docroot/ggzdb
else
	echo " (ok) ggzdb"
fi

if [ ! -d $docroot/grubby ]; then
	echo " (install) grubby"
	cp -r $sysdocroot/grubby $docroot
else
	echo " (ok) grubby"
fi

if [ ! -d $docroot/guides ]; then
	echo " (install) guides"
	mkdir -p $docroot/guides/hosting
	mkdir -p $docroot/guides/gamedev
	cp $sysdocroot/guides/ggz-hosting* $docroot/guides/hosting
	cp $sysdocroot/guides/ggz-game-development* $docroot/guides/gamedev
else
	echo " (ok) guides"
fi

if [ ! -d $docroot/design ]; then
	echo " (install) spec"
	mkdir -p $docroot/design
	mkdir -p $docroot/design/serverspec
	mkdir -p $docroot/design/clientspec
	cp $sysdocroot/spec/design.{pdf,txt} $docroot/design
	cp -r $sysdocroot/spec/design-html $docroot/design/html
	cp -r $sysdocroot/spec/clientspec/* $docroot/design/clientspec
	cp -r $sysdocroot/spec/serverspec/* $docroot/design/serverspec
else
	echo " (ok) spec"
fi

if [ ! -d $docroot/games ]; then
	echo " (install) games"
	mkdir -p $docroot/games
	cp $sysdocroot/games/features.html $docroot/games
	cp $sysdocroot/games/games.html $docroot/games
	cp $sysdocroot/games/external-games/ggzfeatures.html $docroot/games
else
	echo " (ok) games"
fi

if [ ! -d $docroot/games/modules ]; then
	echo " (install) game modules"
	cp -r $sysdocroot/games/modules $docroot/games
else
	echo " (ok) game modules"
fi

if [ ! -d $docroot/coreclients ]; then
	echo " (install) core clients"
	cp -r $sysdocroot/coreclients $docroot
else
	echo " (ok) core clients"
fi

if [ ! -d $docroot/server ]; then
	echo " (install) server"
	mkdir -p $docroot/server
	cp -r $sysdocroot/server/ggzd $docroot/server/ggzd
else
	echo " (ok) server"
fi

if [ ! -d $docroot/overview ]; then
	echo " (install) overview"
	cp -r $sysdocroot/overview $docroot
else
	echo " (ok) overview"
fi

if [ ! -d $docroot/articles ]; then
	echo " (install) articles"
	cp -r $sysdocroot/articles $docroot
else
	echo " (ok) articles"
fi

# Here be documents directly taken from GGZ SVN (some outside of ggz-docs)

echo "Non-installed docs =>"

if [ ! -f $docroot/ggz-debian-depgraph.png ]; then
	echo " (install) depgraph"
	cp $svnroot/playground/maintenance/ggz-debian-depgraph.{ps,png} $docroot
else
	echo " (ok) depgraph"
fi

if [ ! -d $docroot/presentation ]; then
	echo " (install) presentations"
	mkdir -p $docroot/presentation
	cp -r $svnroot/docs/presentation/chemnitz2003 $docroot/presentation
	cp -r $svnroot/docs/presentation/chemnitz2006 $docroot/presentation
else
	echo " (ok) presentations"
fi

# Here be non-automatic docs, needs special checks

echo "Additional docs =>"

### guides as *.txt
### gamedev guide as *.pdf
### talks as *.html

# Here be API documentation, mostly auto-generated

echo "Generated API docs =>"

if [ ! -d $docroot/api/libggz ]; then
	echo " (install) libggz"
	cd $svnroot/libggz && doxygen >/dev/null
	cd $svnroot/libggz/api/latex && make pdf >/dev/null
	cd $origdir
	mkdir -p $docroot/api
	cp -r $svnroot/libggz/api/html $docroot/api/libggz
	cp $svnroot/libggz/api/latex/refman.pdf $docroot/api/libggz.pdf
else
	echo " (ok) libggz"
fi

if [ ! -d $docroot/api/ggzcore ]; then
	echo " (install) ggzcore"
	cd $svnroot/ggz-client-libs/ggzcore && doxygen >/dev/null
	cd $svnroot/ggz-client-libs/ggzcore/api/latex && make pdf >/dev/null
	cd $origdir
	cp -r $svnroot/ggz-client-libs/ggzcore/api/html $docroot/api/ggzcore
	cp $svnroot/ggz-client-libs/ggzcore/api/latex/refman.pdf $docroot/api/ggzcore.pdf
else
	echo " (ok) ggzcore"
fi

if [ ! -d $docroot/api/ggzmod ]; then
	echo " (install) ggzmod"
	cd $svnroot/ggz-client-libs/ggzmod && doxygen >/dev/null
	cd $svnroot/ggz-client-libs/ggzmod/api/latex && make pdf >/dev/null
	cd $origdir
	cp -r $svnroot/ggz-client-libs/ggzmod/api/html $docroot/api/ggzmod
	cp $svnroot/ggz-client-libs/ggzmod/api/latex/refman.pdf $docroot/api/ggzmod.pdf
else
	echo " (ok) ggzmod"
fi

if [ ! -d $docroot/api/ggzdmod ]; then
	echo " (install) ggzdmod"
	cd $svnroot/ggzd/ggzdmod && doxygen >/dev/null
	cd $svnroot/ggzd/ggzdmod/api/latex && make pdf >/dev/null
	cd $origdir
	cp -r $svnroot/ggzd/ggzdmod/api/html $docroot/api/ggzdmod
	cp $svnroot/ggzd/ggzdmod/api/latex/refman.pdf $docroot/api/ggzdmod.pdf
else
	echo " (ok) ggzdmod"
fi

