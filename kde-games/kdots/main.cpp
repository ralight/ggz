///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots_win.h"

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

int main(int argc, char **argv)
{
	KApplication *a;
	KDotsWin *kdotswin;
	KAboutData *about;

	about = new KAboutData("kdots",
		I18N_NOOP("Connect The Dots for KDE"),
		"0.0.4",
		I18N_NOOP("test."),
		KAboutData::License_GPL,
		"Copyright (C) 2001 Josef Spillner",
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://mindx.sourceforge.net",
		"dr_maux@users.sourceforge.net");
	about->addAuthor("Josef Spillner", I18N_NOOP("Author."), "dr_maux@users.sourceforge.net", "http://mindx.sourceforge.net");

	KCmdLineArgs::init(argc, argv, about);

	a = new KApplication();
	kdotswin = new KDotsWin("KDotsWin");
	a->setMainWidget(kdotswin);
	return a->exec();
}

