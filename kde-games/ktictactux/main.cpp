//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 - 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxwin.h"

// GGZ-KDE includes
#include <kggzsystem.h>

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
		I18N_NOOP("KTicTacTux"),
		"0.0.10",
		I18N_NOOP("This is a TicTacToe game for KDE."),
		KAboutData::License_GPL,
		"Copyright (C) 2001 - 2006 Josef Spillner",
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://www.ggzgamingzone.org/gameclients/ktictactux/",
		"josef@ggzgamingzone.org");
	aboutData->addAuthor("Josef Spillner", I18N_NOOP("Inventor"), "josef@ggzgamingzone.org");
	aboutData->setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, aboutData);

	KApplication a;
	KGGZSystem::ensureInstallation();

	ktictactuxwin = new KTicTacTuxWin(NULL, "KTicTacTuxWin");
	if(KGGZMod::Module::isGGZ())
	{
		ktictactuxwin->enableNetwork(true);
	}
	else
	{
		ktictactuxwin->enableNetwork(false);
	}

	a.setMainWidget(ktictactuxwin);
	return a.exec();
}

