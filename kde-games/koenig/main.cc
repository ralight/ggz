// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
// Copyright (C) 2001 - 2004 Josef Spillner, josef@ggzgamingzone.org
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

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include "toplevel.h"

static const KCmdLineOptions op[] =
{
	{"ggz", I18N_NOOP("Request GGZ game explicitely"), 0},
	{0, 0, 0}
};

int main(int argc, char **argv)
{
	KCmdLineArgs *args;
	KAboutData aboutData("koenig",
		"Koenig",
		"0.0.8",
		I18N_NOOP("GGZ Chess Frontend for KDE"),
		KAboutData::License_GPL_V2,
		"Copyright (C) 2001 - 2004 Tobias Koenig & Josef Spillner",
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://www.ggzgamingzone.org/games/koenig/",
		"josef@ggzgamingzone.org");

	aboutData.addAuthor("Tobias KÃ¶nig", "Creator", "tokoe@kde.org");
	aboutData.addAuthor("Josef Spillner", "Maintainer", "josef@ggzgamingzone.org");
	aboutData.setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(op);
	args = KCmdLineArgs::parsedArgs();

	KApplication app;
	TopLevel *toplevel = new TopLevel;
	if(args->isSet("ggz")) toplevel->initGameSocket();
	else toplevel->initLocal();
	toplevel->show();
	app.setMainWidget(toplevel);
	return app.exec();
}

