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

// Keepalive includes
#include "keepalive.h"

// System includes
#include <getopt.h>
#include <cstdlib>
#include <iostream>

using namespace std;

// Main function: parse arguments and start server
int main(int argc, char **argv)
{
	Keepalive *k;
	int ggzmode;
	int optindex;
	int opt;
	struct option options[] =
	{
		{"ggz", no_argument, 0, 'g'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

	ggzmode = 0;

	while(1)
	{
		opt = getopt_long(argc, argv, "gh", options, &optindex);
		if(opt == -1) break;
		switch(opt)
		{
			case 'g':
				ggzmode = 1;
				break;
			case 'h':
				cout << "Keepalive Server 0.1" << endl;
				cout << "Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net" << endl;
				cout << "Pusblished under GNU GPL conditions" << endl << endl;
				cout << "[-h | --help]: Show this help" << endl;
				cout << "[-g | --ggz]: Request GGZ mode" << endl;
				exit(0);
				break;
			default:
				cout << "Unknown command line argument, try --help." << endl;
				exit(-1);
				break;
		}
	}

	if(!ggzmode)
	{
		cout << "Only GGZ mode is currently supported." << endl;
		exit(-1);
	}

	k = new Keepalive();
	k->loop();
	delete k;

	return 0;
}

