#include "krosswater.h"

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	KApplication *a;
	KAboutData *about;
	Krosswater *krosswater;
	static const KCmdLineOptions op[] = {{"o", I18N_NOOP("Launch game instead of joining"), 0}, {0, 0, 0}};

	cout << "Krosswater: starting" << endl;

	about = new KAboutData("krosswater",
		"Krosswater GGZ game",
		"0.0.1",
		"",
		KAboutData::License_GPL,
		"(C) 2001 Josef Spillner",
		"",
		"http://mindx.sourceforge.net",
		"dr_maux@users.sourceforge.net");

	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions(op);

	cout << "Krosswater: loading application" << endl;

	a = new KApplication(argc, argv);
	krosswater = new Krosswater(NULL, "Krosswater");
	a->setMainWidget(krosswater);
	return a->exec();
}
