// Widelands server for GGZ
// Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>
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

// Widelands includes
#include "widelands_server.h"

// System includes
#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <ctime>

// Version information
#define WIDELANDS_SERVER_VERSION "0.1"

// Possible server startup modes
#define MODE_NORMAL 1
#define MODE_GGZ 2

// Main function: parse arguments and start server
int main(int argc, char** argv)
{
	int option;
	struct option opt [] =
	{
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"ggz", no_argument, NULL, 'g'},
		{NULL, 0, NULL, 0}
	};
	int optindex;
	int mode;

	mode = MODE_NORMAL;

	while((option = getopt_long(argc, argv, "hvg", opt, &optindex)) != EOF)
		switch(option)
		{
			case 'h':
				std::cout << "The GGZ Gaming Zone Widelands Server" << std::endl
					<< "Copyright (C) 2004 Josef Spillner <josef@ggzgamingzone.org>" << std::endl
					<< "Published under GNU GPL conditions " << std::endl << std::endl
					<< "Options: " << std::endl
					<< "[-h | --help]    This help screen" << std::endl
					<< "[-v | --version] Version information" << std::endl
					<< "[-g | --ggz]     Start in GGZ mode" << std::endl;
				exit(EXIT_SUCCESS);
			case 'v':
				std::cout << "GGZ Widelands Server version " WIDELANDS_SERVER_VERSION << std::endl;
				exit(EXIT_SUCCESS);
			case 'g':
				mode = MODE_GGZ;
				break;
		}

	srand(time(NULL));

	if(mode == MODE_GGZ)
	{
		WidelandsServer *wls = new WidelandsServer();
		wls->connect(false);
		delete wls;
	}
	else
	{
		std::cout << "Currently only GGZ mode is supported." << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}

