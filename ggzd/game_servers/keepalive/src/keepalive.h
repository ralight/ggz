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

#ifndef KEEPALIVE_H
#define KEEPALIVE_H

#define GGZSPECTATORS

// Class declarations
class World;

// Keepalive server object
class Keepalive
{
	public:
		Keepalive();
		~Keepalive();
		void loop();
		void hookData(const void *data);
		void hookJoin(const void *data);
		void hookLeave(const void *data);
		void hookSpectatorJoin(const void *data);
		void hookSpectatorLeave(const void *data);
		void hookSpectatorData(const void *data);
		void hookState(const void *data);
		void hookError(const void *data);
		void hookLog(const void *data);

		int m_valid;

	private:
		World *m_world;
};

#endif

