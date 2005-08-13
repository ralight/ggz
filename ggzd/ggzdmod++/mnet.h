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

/* Multicast-like derivate from class Net */
class MNet : public Net
{
	public:
		MNet();
		~MNet();

		/* These opcodes maintain the set of receivers:
		 * add - add a file descriptor to set
		 * remove - remove a file descriptor from set
		 * clear - reset the whole set
		 * All 3 opcodes require the fd as following argument.
		 * In the case of clear, simply 0 should be given.
		 */
		enum ChannelOpcodes
		{
			add = -2001,
			remove = -2002,
			clear = -2003
		};

		/* These opcodes toggle between the possible modes.
		 * peer - send only to one peer (which must be given after switching)
		 * multicast - send to configured set of peers
		 * broadcast - not implemented
		 */
		enum ModeOpcodes
		{
			peer = -3001,
			multicast = -3002,
			broadcast = -3003
		};

		/* Configure the multicast set */
		MNet &operator<<(ChannelOpcodes value);
		/* Configure the mode of operation */
		MNet &operator<<(ModeOpcodes value);
		/* Reimplemented from Net: define object behaviour */
		MNet &operator<<(Net::SpecialOpcodes value);

		/* Reimplemented from Net: inject integer value */
		MNet &operator<<(int value);
		/* Reimplemented from Net: inject string value */
		MNet &operator<<(const char* value);

	private:
		int m_mode;
		int waitforchannel;
		std::list<int> channellist;
		std::list<int>::iterator it;
};

#endif

