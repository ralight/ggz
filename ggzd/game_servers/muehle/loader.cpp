// Muehle - KDE Muehle (Nine Men's Morris) game for GGZ
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

// Header file
#include "loader.h"

// Configuration includes
#include "config.h"

// System includes
#include <fstream>
#include <string>
#include <iostream>

// Static function to load a qweb
QWeb *MuehleLoader::loadVariant ( const char *file ) {
	std::fstream f;
	std::string s;
	QWeb *web;
	char buffer[ 256 ];

	s = GGZDDATADIR;
	s.append ( "/muehle/" );
	s.append ( file );

	f.open ( s.data(), std::ios::in );
	if ( !f.is_open () ) {
		return NULL;
	}

	web = new QWeb();

	// Format: "(x1, y1), (x2, y2)\n"
	while ( !f.eof() ) {
		f.getline ( buffer, sizeof ( buffer ) );
		s = buffer;

		std::cout << "Load: " << s << std::endl;
	}
	f.close ();

	return web;
}

