// Keepalive - Experimental GGZ game
// Copyright (C) 2002 Josef Spillner, josef@ggzgamingzone.org
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
#include <kapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

// Keepalive includes
#include "win.h"

// System includes
#include <iostream>
#include <cstdlib>

// Command line arguments
static const KCmdLineOptions op[] =
{
	{"ggz", I18N_NOOP("Start game in GGZ mode"), 0},
	{0, 0, 0}
};

// Main function: fire up the top-level window
int main(int argc, char **argv)
{
	Win *win;
	KCmdLineArgs *args;
	KAboutData *about;
	int ggz = 0;

	about = new KAboutData("keepalive",
		"Keepalive",
		"0.1",
		I18N_NOOP("An experimental game for GGZ"),
		KAboutData::License_GPL,
		"Copyright (C) 2002 Josef Spillner",
		I18N_NOOP(""),
		"http://www.ggzgamingzone.org/games/keepalive/",
		"josef@ggzgamingzone.org");
	about->addAuthor("Josef Spillner", I18N_NOOP("Initial developer"), "josef@ggzgamingzone.org");
	about->setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, about);
	KCmdLineArgs::addCmdLineOptions(op);
	args = KCmdLineArgs::parsedArgs();

	if(args->isSet("ggz")) ggz = 1;

	KApplication a;
	win = new Win();

	if(!ggz)
	{
		//std::cerr << i18n("This game cannot be launched from the command line.").latin1() << std::endl;
		//std::cerr << i18n("Please use a GGZ core client.").latin1() << std::endl;
		//exit(-1);
		std::cout << "Network mode disabled" << std::endl;
	}
	else
	{
		std::cout << "Network mode enabled" << std::endl;
		win->init();
	}

	a.setMainWidget(win);
	return a.exec();
}

