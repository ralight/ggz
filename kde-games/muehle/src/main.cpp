#include <kapp.h>
#include <klocale.h>
#include <kaboutdata.h>
#include "toplevel.h"
#include <kcmdlineargs.h>
#include <iostream>

int main(int argc, char **argv)
{
	Toplevel *top;
	KAboutData *about;
	KCmdLineArgs *args;
	static const KCmdLineOptions op[] = {
		{"ggz", I18N_NOOP("Request GGZ game explicitely"), 0},
		{0, 0, 0}};

	about = new KAboutData("muehle", "KDE Muehle Game", "0.1", "Muehle game for KDE",
		KAboutData::License_GPL, "Copyright (C) 2001 Josef Spillner", "This game supports multiple variants of Muehle.",
		"http://games.kde.org", "dr_maux@users.sourceforge.net");

	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions(op);
	args = KCmdLineArgs::parsedArgs();

	if(!args->isSet("ggz"))
	{
		cout << "Sorry, this game does only work in GGZ mode so far." << endl;
		return -1;
	}

	KApplication a;

	top = new Toplevel();
	a.setMainWidget(top);
	return a.exec();
}

