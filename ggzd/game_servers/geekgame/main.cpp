// Geekgame includes
#include "geekgame.h"

// System includes
#include <iostream>
#include <getopt.h>
#include <cstdlib>

// Version information
#define GEEKGAME_VERSION "0.1"

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
				std::cout << "The GGZ Gaming Zone Geekgame Server" << std::endl
					<< "Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net" << std::endl
					<< "Published under GNU GPL conditions " << std::endl << std::endl
					<< "Options: " << std::endl
					<< "[-h | --help]    This help screen" << std::endl
					<< "[-v | --version] Version information" << std::endl
					<< "[-g | --ggz]     Start in GGZ mode" << std::endl;
				exit(EXIT_SUCCESS);
			case 'v':
				std::cout << "GGZ Geekgame Server version " GEEKGAME_VERSION << std::endl;
				exit(EXIT_SUCCESS);
			case 'g':
				mode = MODE_GGZ;
				break;
		}

	if(mode == MODE_GGZ)
	{
		Geekgame *geek = new Geekgame();
		geek->connect();
		delete geek;
	}
	else
	{
		std::cout << "Currently only GGZ mode is supported." << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}

