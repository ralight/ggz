// Krosswater - Cross The Water for KDE
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// Krosswater includes
#include "krosswater.h"

// KDE includes
#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

// System includes
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
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://ggz.sourceforge.net/games/krosswater/",
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

