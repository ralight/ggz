//
//    Keepalive Control
//
//    Copyright (C) 2002 Josef Spillner <dr_maux@users.sourceforge.net>
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
#include "config.h"

int main(int argc, char **argv)
{
	Toplevel *top;
	KAboutData *about;

	about = new KAboutData("keepalivecontrol",
		I18N_NOOP("Keepalive Control"),
		VERSION,
		I18N_NOOP("Keepalive Server Control Tool"),
		KAboutData::License_GPL,
		"Copyright (C) 2002 Josef Spillner",
		QString::null,
		"http://ggz.sourceforge.net/games/keepalive/",
		"dr_maux@users.sourceforge.net");

	KCmdLineArgs::init(argc, argv, about);

	KApplication a;
	top = new Toplevel();
	a.setMainWidget(top);
	return a.exec();
}

