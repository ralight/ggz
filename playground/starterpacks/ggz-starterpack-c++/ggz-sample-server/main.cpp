//
// GGZ Starterpack for C++ - Sample Server
// Copyright (C) 2008 GGZ Development Team
//
// This code is made available as public domain; you can use it as a base
// for your own game, as long as its licence is compatible with the libraries
// you use.

#include "game.h"

#include <QCoreApplication>

#include <kcomponentdata.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kdebug.h>

int main(int argc, char **argv)
{
	KAboutData about("ggz-sample-server-c++",
		QByteArray(),
		ki18n("GGZ Starterpack for C++ - Sample Server"),
		"0.1",
		ki18n("A kggzdmod-based game server"),
		KAboutData::License_GPL_V2,
		ki18n("Copyright (C) You!"),
		KLocalizedString(),
		"http://www.ggzgamingzone.org/"
	);
	KComponentData cd(about);

	QCoreApplication a(argc, argv);

	if(!KGGZdMod::Module::isGGZ())
	{
		qFatal("Error: Game server only runs in GGZ mode.");
		return 1;
	}

	Game *game = new Game();
	Q_UNUSED(game);

	return a.exec();
}

