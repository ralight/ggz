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

#include "mnet.h"
#include <iostream> // cout

MNet::MNet()
: Net()
{
	waitforchannel = 0;
	m_mode = peer;
}

MNet::~MNet()
{
}

MNet &MNet::operator<<(ChannelOpcodes value)
{
	waitforchannel = value;
	return *this;
}

MNet &MNet::operator<<(ModeOpcodes value)
{
	m_mode = value;
	//std::cout << "[mnet] mode " << value << std::endl;
	return *this;
}

MNet &MNet::operator<<(SpecialOpcodes value)
{
	if((m_mode != peer) && ((value == begin) || (value == end) || (value == flush)))
	{
			for(it = channellist.begin(); it != channellist.end(); it++)
			{
				//std::cout << "[mnet] broadcasting special to " << (*it) << std::endl;
				Net::operator<<(channel);
				Net::operator<<((*it));
				Net::operator<<(value);
			}
	}
	else Net::operator<<(value);
	return *this;
}

MNet &MNet::operator<<(int value)
{
	if(waitforchannel)
	{
		if(waitforchannel == add)
		{
			channellist.push_back(value);
			std::cout << "[mnet] add fd " << value << std::endl;
		}
		else if(waitforchannel == remove)
		{
			/*for(it = channellist.begin(); it != channellist.end(); it++)
				if((*it) == value)
				{
					channellist.remove((*it));
					break;
				}*/
			channellist.remove(value);
			std::cout << "[mnet] remove fd " << value << std::endl;
		}
		else if(waitforchannel == clear)
		{
			channellist.clear();
			std::cout << "[mnet] clear fds " << std::endl;
		}
		waitforchannel = 0;
	}
	else
	{
		if(m_mode == peer) Net::operator<<(value);
		else
		{
			for(it = channellist.begin(); it != channellist.end(); it++)
			{
				std::cout << "[mnet] broadcasting to " << (*it) << std::endl;
				Net::operator<<(channel);
				Net::operator<<((*it));
				Net::operator<<(value);
			}
		}
	}
	return *this;
}

MNet &MNet::operator<<(const char* value)
{
	if(m_mode == peer) Net::operator<<(value);
	else
	{
		for(it = channellist.begin(); it != channellist.end(); it++)
		{
			//std::cout << "[mnet] broadcasting to " << (*it) << std::endl;
			Net::operator<<(channel);
			Net::operator<<((*it));
			Net::operator<<(value);
		}
	}
	return *this;
}


