// Networking class (broadcast add-on) for ggzdmod++ library
// Copyright (C) 2003 - 2005 Josef Spillner <josef@ggzgamingzone.org>
//
// This library has its origin in kamikaze/server!
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

#ifndef GGZDMODPP_MNET_H
#define GGZDMODPP_MNET_H

#include <net.h>
#include <list>

class MNet : public Net
{
	public:
		MNet();
		~MNet();

		enum ChannelOpcodes
		{
			add = -2001,
			remove = -2002,
			clear = -2003
		};

		enum ModeOpcodes
		{
			peer = -3001,
			multicast = -3002,
			broadcast = -3003
		};

		MNet &operator<<(ChannelOpcodes value);
		MNet &operator<<(ModeOpcodes value);
		MNet &operator<<(Net::SpecialOpcodes value);

		MNet &operator<<(int value);
		MNet &operator<<(const char* value);

	private:
		int m_mode;
		int waitforchannel;
		std::list<int> channellist;
		std::list<int>::iterator it;
};

#endif

