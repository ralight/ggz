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

// Header file
#include "spectator.h"

// System includes
#include <cstring>
#include <cstdlib>

using namespace std;

// Constructor
Spectator::Spectator(const char *name, int fd)
{
	m_name = strdup(name);
	m_fd = fd;
}

// Destructor
Spectator::~Spectator()
{
	free(m_name);
}

// Return spectator's fd
int Spectator::fd()
{
	return m_fd;
}

// Return spectator's name
char *Spectator::name()
{
	return m_name;
}

