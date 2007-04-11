#!/bin/sh
#
# Create diffs between a release and the branch

svnroot=svn://svn.ggzgamingzone.org/svn
branch=ggz-0-0-14

modules="libggz ggz-client-libs ggzd docs gtk-client gtk-games kde-client kde-games grubby utils python java txt-client gnome-client sdl-games community"

for module in $modules; do
	package=$(echo $module | sed -e 's/ggzd/ggz-server/;s/gtk-client/ggz-gtk-client/;s/gtk-games/ggz-gtk-games/;s/kde-games/ggz-kde-games/;s/kde-client/ggz-kde-client/;s/grubby/ggz-grubby/;s/utils/ggz-utils/;s/python/ggz-python/;s/java/ggz-java/;s/txt-client/ggz-txt-client/;s/gnome-client/ggz-gnome-client/;s/sdl-games/ggz-sdl-games/;s/community/ggz-community/')

	echo "$module ($package) [$branch]"
	difffile=$package-branch-$branch.diff

	svn diff \
		$svnroot/tags/$branch/$module \
		$svnroot/branches/$branch/$module \
		> $difffile

	lines=$(wc -l $package-branch-$branch.diff | cut -d " " -f 1)
	if [ $lines = "0" ]; then
		rm -f $difffile
	else
		mkdir -p public_html
		mv $difffile public_html
	fi
done

