// GGZap - GGZ quick start application for the KDE panel
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

// GGZap includes
#include "ggzap.h"

// KDE includes
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

// Main function
int main(int argc, char **argv)
{
	GGZap *zap;
	KAboutData *aboutData;
	KCmdLineArgs *args;
	static const KCmdLineOptions op[] = {
		{"module <modulename>", I18N_NOOP("Specify module to launch"), 0},
		{"frontend <frontendtype>", I18N_NOOP("Preferred frontend"), 0},
		{0, 0, 0}
	};

	aboutData = new KAboutData("ggzap",
		I18N_NOOP("GGZ Quick Launcher"),
		"0.2",
		I18N_NOOP("Tool to start GGZ games directly."),
		KAboutData::License_GPL,
		"(C) 2000 - 2002 Josef Spillner\n"
		"The MindX Open Source Project",
		I18N_NOOP("GGZap"),
		"http://ggz.sourceforge.net/coreclients/ggzap",
		"dr_maux@users.sourceforge.net");

	aboutData->addAuthor("Josef Spillner", I18N_NOOP("Author"), "dr_maux@users.sourceforge.net", "http://mindx.sourceforge.net");

	KCmdLineArgs::init(argc, argv, aboutData);
	KCmdLineArgs::addCmdLineOptions(op);

	args = KCmdLineArgs::parsedArgs();

	KApplication a;
	zap = new GGZap();
	if(args->isSet("module")) zap->setModule(args->getOption("module"));
	if(args->isSet("frontend")) zap->setFrontend(args->getOption("frontend"));
	zap->launch();
	a.setMainWidget(zap->gui());
	return a.exec();
}

