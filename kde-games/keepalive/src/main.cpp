// Keepalive - Experimental GGZ game
// Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
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

// Qt includes
#include <qapplication.h>

// Keepalive includes
#include "win.h"

// System includes
#include <iostream>
#include <cstdlib>

// Main function: fire up the top-level window
int main(int argc, char **argv)
{
	Win *win;
	int ggz = 0;

	for(int i = 1; i < argc; i++)
		if(!strcmp(argv[i], "--ggz")) ggz = 1;
	if(!ggz)
	{
		cerr << "This game cannot be launched from the command line." << endl;
		cerr << "Please use a GGZ core client." << endl;
		exit(-1);
	}

	QApplication a(argc, argv);
	win = new Win();
	a.setMainWidget(win);
	return a.exec();
}

