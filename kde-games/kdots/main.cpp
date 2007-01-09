///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots_win.h"

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <iostream>

int main(int argc, char **argv)
{
	KDotsWin *kdotswin;
	KAboutData *about;
	KCmdLineArgs *args;
	static const KCmdLineOptions op[] = {
		{"ggz", I18N_NOOP("Request GGZ game explicitely"), 0},
		{0, 0, 0}};

	about = new KAboutData("kdots",
		I18N_NOOP("KDots"),
		"0.0.8",
		I18N_NOOP("Connect The Dots for KDE"),
		KAboutData::License_GPL,
		"Copyright (C) 2001 - 2004 Josef Spillner",
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://www.ggzgamingzone.org/gameclients/kdots/",
		"josef@ggzgamingzone.org");
	about->addAuthor("Josef Spillner", I18N_NOOP("Author."), "josef@ggzgamingzone.org");
	about->setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions(op);
	args = KCmdLineArgs::parsedArgs();

	if(!args->isSet("ggz"))
	{
		std::cout << "Running in replay-only mode." << std::endl;
	}

	KApplication a;
	kdotswin = new KDotsWin(args->isSet("ggz"));
	a.setMainWidget(kdotswin);
	return a.exec();
}

