// ShadowBridge - Game developer tool to visualize network protocols
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

// ShadowBridge includes
#include "shadowapp.h"

// KDE includes
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

// Main function
int main(int argc, char **argv)
{
	ShadowApp *app;
	KAboutData *aboutData;
	/*KCmdLineArgs *args;*/

	aboutData = new KAboutData("shadowbridge",
		I18N_NOOP("GGZ Shadow Bridge"),
		"0.1",
		I18N_NOOP("Drop-in fake game client to record\nand distribute game events."),
		KAboutData::License_GPL,
		"Copyright (C) 2001, 2002 Josef Spillner\n"
		"The MindX Open Source Project",
		I18N_NOOP("ShadowBridge"),
		"http://www.ggzgamingzone.org/",
		"josef@ggzgamingzone.org");

	aboutData->addAuthor("Josef Spillner", I18N_NOOP("Author"), "josef@ggzgamingzone.org");

	KCmdLineArgs::init(argc, argv, aboutData);

	KApplication a;
	app = new ShadowApp();
	a.setMainWidget(app);
	return a.exec();
}

