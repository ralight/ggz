//
// GGZ Starterpack for C++ - Sample Client
// Copyright (C) 2008 GGZ Development Team
//
// This code is made available as public domain; you can use it as a base
// for your own game, as long as its licence is compatible with the libraries
// you use.

#include "gamewin.h"

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kggzmod/module.h>

int main(int argc, char **argv)
{
	KAboutData *aboutData;
	GameWin *gamewin;

	aboutData = new KAboutData("ggz-sample-client-cpp",
		0,
		ki18n("GGZ Starterpack for C++ - Sample Client"),
		"0.1",
		ki18n("A kggzmod-based game client."),
		KAboutData::License_GPL,
		ki18n("Copyright (C) You!"),
		KLocalizedString(),
		"http://www.ggzgamingzone.org/");

	KCmdLineArgs::init(argc, argv, aboutData);

	KApplication a;

	gamewin = new GameWin();
	if(KGGZMod::Module::isGGZ())
	{
		gamewin->enableNetwork(true);
	}
	else
	{
		gamewin->enableNetwork(false);
	}

	return a.exec();
}

