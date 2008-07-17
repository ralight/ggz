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
#include "kdots_replay.h"

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

int main(int argc, char **argv)
{
	KDotsWin *kdotswin;
	KDotsReplay *kdotsreplay;
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
	about->addAuthor(ki18n("Josef Spillner"), ki18n("Author"), "josef@ggzgamingzone.org");

	KCmdLineOptions options;
	options.add("replay <file>", ki18n("Savegame file for replay"));
	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication a;

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if(args->isSet("replay"))
	{
		kdotsreplay = new KDotsReplay();
		kdotsreplay->replay(args->getOption("replay"));
	}
	else
	{
		kdotswin = new KDotsWin(KGGZMod::Module::isGGZ());
	}

	return a.exec();
}

