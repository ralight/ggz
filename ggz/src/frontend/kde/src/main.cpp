/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4                //
//    Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net      //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// Main: Setup the whole application and wait for a return value. Essential part!! //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// KGGZ includes
#include "KGGZCommon.h"
#include "KGGZBase.h"

// KDE includes
#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

// What's that? Right: The main function.
int main(int argc, char **argv)
{
	KApplication *a;
	KGGZBase *kggzbase;
	KAboutData *aboutData;

	// this is shown in the About dialog
	aboutData = new KAboutData("kggz",
		I18N_NOOP("KGGZ"),
		KGGZVERSION,
		I18N_NOOP("Your most desired KDE network games launcher and chat system."),
		KAboutData::License_GPL,
		"(C) 2000,2001 Josef Spillner\n"
		"The MindX Open Source Project",
		I18N_NOOP("KGGZ is a frontend for the GGZ Gaming Zone"),
		"http://mindx.sourceforge.net/games/kggz",
		"dr_maux@users.sourceforge.net");

	// forgotten anybody? tell me.
	aboutData->addAuthor("Josef Spillner", I18N_NOOP("KDE2 port of the client"), "dr_maux@users.sourceforge.net", "http://mindx.sourceforge.net");
	aboutData->addCredit("Brian Cox", I18N_NOOP("(GGZ team)"), "bcox@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Rich Gade", I18N_NOOP("(GGZ team)"), "rgade@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Brent Hendricks", I18N_NOOP("(GGZ team)"), "bmh@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Justin Zaun", I18N_NOOP("(GGZ team)"), "jzaun@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Doug Hudson", I18N_NOOP("(GGZ team)"), "djh@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Ismael Orenstein", I18N_NOOP("(GGZ team)"), "perdig@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Dan Papasian", I18N_NOOP("(GGZ team)"), "bugg@users.sourceforge.net", "http://ggz.sourceforge.net");
	aboutData->addCredit("Ricardo Quesada", I18N_NOOP("(GGZ team)"), "riq@core-sdi.com", "http://teg.sourceforge.net");

	// process command line options
	KCmdLineArgs::init(argc, argv, aboutData);

	// some people don't like references, but I do
	a = new KApplication();
	kggzbase = new KGGZBase();
	a->setMainWidget(kggzbase);
	return a->exec();
}
