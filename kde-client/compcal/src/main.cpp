//
//    Competition Calendar
//
//    Copyright (C) 2002 - 2004 Josef Spillner <josef@ggzgamingzone.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <kapplication.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include "toplevel.h"

int main(int argc, char **argv)
{
	Toplevel *top;
	KAboutData *about;

	about = new KAboutData("compcal",
		I18N_NOOP("Competition Calendar"),
		"0.3",
		I18N_NOOP("Online game scheduling system"),
		KAboutData::License_GPL,
		"Copyright (C) 2002 - 2004 Josef Spillner",
		I18N_NOOP("Manages GGZ Gaming Zone tournaments and challenges"),
		"http://www.ggzgamingzone.org/clients/compcal/",
		"josef@ggzgamingzone.org");
	about->addAuthor("Josef Spillner", I18N_NOOP("Developer"), "josef@ggzgamingzone.org");
	about->setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, about);

	KApplication a;
	top = new Toplevel();
	a.setMainWidget(top);
	return a.exec();
}

