/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#include "kdots_win.h"

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

int main(int argc, char **argv)
{
	KDotsWin *kdotswin;
	KAboutData *about;

	about = new KAboutData("kdots",
		0,
		ki18n("KDots"),
		"0.99.4",
		ki18n("Connect The Dots for KDE"),
		KAboutData::License_GPL,
		ki18n("Copyright (C) 2001 - 2008 Josef Spillner"),
		ki18n("This game is part of the GGZ Gaming Zone."),
		"http://www.ggzgamingzone.org/gameclients/kdots/",
		"josef@ggzgamingzone.org");
	about->addAuthor(ki18n("Josef Spillner"), ki18n("Author."), "josef@ggzgamingzone.org");

	KCmdLineArgs::init(argc, argv, about);

	KApplication a;

	kdotswin = new KDotsWin(KGGZMod::Module::isGGZ());

	return a.exec();
}

