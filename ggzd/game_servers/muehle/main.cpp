#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include "muehleserver.h"

#define MUEHLE_VERSION "0.1"

#define MODE_NORMAL 1
#define MODE_GGZ 2

int
main ( int argc, char** argv) {
	char option;
	struct option opt [] = {
		{ "help", no_argument, NULL, 'h' },
		{ "version", no_argument, NULL, 'v' },
		{ "ggz", no_argument, NULL, 'g' },
		{ NULL, 0, NULL, 0 }
	};
	int optindex;
	int mode;

	mode = MODE_NORMAL;

	while (( option = getopt_long ( argc, argv, "hvg", opt, &optindex )) != EOF )
		switch ( option ) {
			case 'h':
				std::cout << "The GGZ Gaming Zone Muehle Server" << std::endl
					<< "Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net" << std::endl
					<< "Published under GNU GPL conditions " << std::endl << std::endl
					<< "Options: " << std::endl
					<< "[-h | --help]    This help screen" << std::endl
					<< "[-v | --version] Version information" << std::endl
					<< "[-g | --ggz]     Start in GGZ mode" << std::endl;
				exit ( EXIT_SUCCESS );
			case 'v':
				std::cout << "GGZ Muehle Server version " MUEHLE_VERSION << std::endl;
				exit ( EXIT_SUCCESS );
			case 'g':
				mode = MODE_GGZ;
				break;
		}

	if ( mode == MODE_GGZ ) {
		MuehleServer *serv = new MuehleServer ();
		serv->connect ();
		delete serv;
	}

	return 0;
}

