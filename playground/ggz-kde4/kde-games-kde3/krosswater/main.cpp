// Krosswater - Cross The Water for KDE
// Copyright (C) 2001, 2002 Josef Spillner, josef@ggzgamingzone.org
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
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

// System includes
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	KAboutData *about;
	Krosswater *krosswater;
	static const KCmdLineOptions op[] = {
		{"ggz", I18N_NOOP("Request GGZ game explicitly"), 0},
		{0, 0, 0}
	};

	about = new KAboutData("krosswater",
		I18N_NOOP("Krosswater"),
		"0.0.6",
		I18N_NOOP("GGZ version of Cross The Water"),
		KAboutData::License_GPL,
		"(C) 2001, 2002 Josef Spillner",
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://www.ggzgamingzone.org/gameclients/krosswater/",
		"josef@ggzgamingzone.org");

	about->addAuthor("Josef Spillner", I18N_NOOP("Initial developer"), "josef@ggzgamingzone.org");
	about->setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions(op);
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	if(!args->isSet("ggz"))
	{
		cout << "Sorry, this game does only work in GGZ mode so far." << endl;
		exit(-1);
	}

	KApplication a;
	krosswater = new Krosswater(NULL, "Krosswater");
	a.setMainWidget(krosswater);
	return a.exec();
}

