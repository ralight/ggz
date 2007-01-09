//////////////////////////////////////////////////////////////////////
// KConnectX
// Copyright (C) 2006 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KConnectX includes
#include "win.h"

// GGZ-KDE-Games includes
#include <kggzmod/module.h>
#include <kggzsystem.h>

// KDE includes
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	KAboutData about("kconnectx",
		I18N_NOOP("KConnectX"),
		"0.0.1",
		I18N_NOOP("A ConnectX game for KDE."),
		KAboutData::License_GPL,
		"Copyright (C) 2006 Josef Spillner",
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://www.ggzgamingzone.org/gameclients/kconnectx/",
		"josef@ggzgamingzone.org");
	about.addAuthor("Josef Spillner", I18N_NOOP("Inventor"), "josef@ggzgamingzone.org");
	about.setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, &about);
	KApplication a;

	KGGZSystem::ensureInstallation();

	if(!KGGZMod::Module::isGGZ())
	{
		qWarning("Error: kconnectx must run from GGZ client!");
		return -1;
	}

	Win *win = new Win();

	a.setMainWidget(win);
	return a.exec();
}

