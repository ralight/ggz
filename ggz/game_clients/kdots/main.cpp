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

#include "kdots.h"

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

int main(int argc, char **argv)
{
	KApplication *a;
	KDots *kdots;
	KAboutData *about;

	about = new KAboutData("", "", "");
	KCmdLineArgs::init(argc, argv, about);

	a = new KApplication();
	kdots = new KDots(NULL, "KDots");
	a->setMainWidget(kdots);
	return a->exec();
}
