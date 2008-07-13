//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxwin.h"

// GGZ-KDE includes
//#include <kggzsystem.h>

// KDE includes
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	KAboutData *aboutData;
	KTicTacTuxWin *ktictactuxwin;

	aboutData = new KAboutData("ktictactux",
		0,
		ki18n("KTicTacTux"),
		"0.99.4",
		ki18n("This is a TicTacToe game for KDE."),
		KAboutData::License_GPL,
		ki18n("Copyright (C) 2001 - 2006 Josef Spillner"),
		ki18n("This game is part of the GGZ Gaming Zone."),
		"http://www.ggzgamingzone.org/gameclients/ktictactux/",
		"josef@ggzgamingzone.org");
	aboutData->addAuthor(ki18n("Josef Spillner"), ki18n("Inventor"), "josef@ggzgamingzone.org");

	KCmdLineArgs::init(argc, argv, aboutData);

	KApplication a;
	//KGGZSystem::ensureInstallation();

	ktictactuxwin = new KTicTacTuxWin();
	if(KGGZMod::Module::isGGZ())
	{
		ktictactuxwin->enableNetwork(true);
	}
	else
	{
		ktictactuxwin->enableNetwork(false);
	}

	return a.exec();
}

