#include "krosswater.h"

#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <stdlib.h>

#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	KAboutData *about;
	Krosswater *krosswater;
	static const KCmdLineOptions op[] = {
		{"ggz", I18N_NOOP("Request GGZ game explicitely"), 0},
		{0, 0, 0}
	};

	cout << "Krosswater: starting" << endl;

	about = new KAboutData("krosswater",
		"Krosswater",
		"0.0.1",
		I18N_NOOP("GGZ version of Cross The Water"),
		KAboutData::License_GPL,
		"(C) 2001, 2002 Josef Spillner",
		I18N_NOOP("This game is part of the GGZ project."),
		"http://mindx.sourceforge.net",
		"dr_maux@users.sourceforge.net");

	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions(op);
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	if(!args->isSet("ggz"))
	{
		cout << "Sorry, this game does only work in GGZ mode so far." << endl;
		exit(-1);
	}

	cout << "Krosswater: loading application" << endl;

	KApplication a;
	krosswater = new Krosswater(NULL, "Krosswater");
	a.setMainWidget(krosswater);
	return a.exec();
}

