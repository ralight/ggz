// Muehle - KDE Muehle (Nine Men's Morris) game for GGZ
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

// KDE includes
#include <kapp.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

// Muehle includes
#include "toplevel.h"

// System includes
#include <iostream>

// Main functions: start game with the given options
int main(int argc, char **argv)
{
	Toplevel *top;
	KAboutData *about;
	KCmdLineArgs *args;
	static const KCmdLineOptions op[] = {
		{"ggz", I18N_NOOP("Request GGZ game explicitely"), 0},
		{0, 0, 0}};

	about = new KAboutData("muehle",
		I18N_NOOP("KDE Muehle Game"),
		"0.1",
		I18N_NOOP("Muehle game for KDE supporting multiple variants"),
		KAboutData::License_GPL,
		"Copyright (C) 2001, 2002 Josef Spillner",
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://ggz.sourceforge.net/games/muehle/",
		"dr_maux@users.sourceforge.net");
	about->addAuthor("Josef Spillner", "Programming", "dr_maux@users.sourceforge.net");

	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions(op);
	args = KCmdLineArgs::parsedArgs();

	KApplication a;

	top = new Toplevel();
	if(args->isSet("ggz"))
	{
		top->enableNetwork(true);
	}
	else
	{
		top->enableNetwork(false);
	}
	a.setMainWidget(top);
	return a.exec();
}

