#include <kapp.h>
#include <klocale.h>
#include <kaboutdata.h>
#include "toplevel.h"
#include <kcmdlineargs.h>

int main(int argc, char **argv)
{
	Toplevel *top;
	KAboutData *about;

	about = new KAboutData("muehle", "muehle", "0.1", "Muehle game for KDE",
		KAboutData::License_GPL, "Copyright (C) 2001 Josef Spillner", "???",
		"http://games.kde.org", "dr_maux@users.sourceforge.net");
	KCmdLineArgs::init(argc, argv, about);
	KApplication a;

	top = new Toplevel();
	a.setMainWidget(top);
	return a.exec();
}

