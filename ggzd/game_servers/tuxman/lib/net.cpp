// Tuxman Server
// Copyright (C) 2003, 2004 Josef Spillner <josef@ggzgamingzone.org>
// Tuxman Copyright (C) 2003 Robert Strobl <badwolf@acoderz.de>
//
// This library has been taken from kamikaze/server!
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

#include "net.h"
#include <ggz.h> // ggz_read*, ggz_write*, ggz_malloc
#include <unistd.h> // read, write
#include <string.h> // memmove
#include <netinet/in.h> // ntohl
#include <iostream> // cout, cerr

Net::Net()
{
	m_fd = -1;
	waitforchannel = 0;
	buffered = 0;
}

Net::~Net()
{
}

void Net::forceinput()
{
	int ret = read(m_fd, queues[m_fd].receive + queues[m_fd].receivepos, sizeof(queues[m_fd].receive) - queues[m_fd].receivepos);
	if(ret >= 0) queues[m_fd].receivepos += ret;
	else std::cerr << "[net] error during read from fd " << m_fd << std::endl;
	std::cout << "[net] received bytes: " << ret << " position is now: " << queues[m_fd].receivepos << std::endl;
}

Net &Net::operator<<(SpecialOpcodes value)
{
	switch(value)
	{
		case begin:
			buffered = 1;
			break;
		case end:
			buffered = 0;
			break;
		case flush:
			write(m_fd, queues[m_fd].send, queues[m_fd].sendpos);
			//std::cout << "[net] flushed bytes: " << queues[m_fd].sendpos << std::endl;
			queues[m_fd].sendpos = 0;
			break;
		case channel:
			if(!waitforchannel) waitforchannel = 1;
			break;
		case input:
			forceinput();
			break;
	}
	return *this;
}

Net &Net::operator<<(int value)
{
	if(waitforchannel)
	{
		waitforchannel = 0;
		m_fd = value;
		//std::cout << "[net] assign fd: " << m_fd << std::endl;
	}
	else
	{
		//std::cout << "[net] write int: " << value << std::endl;
		if(m_fd >= 0)
		{
			if(buffered)
			{
				value = htonl(value);
				memmove(queues[m_fd].send + queues[m_fd].sendpos, &value, sizeof(int));
				queues[m_fd].sendpos += sizeof(int);
			}
			else ggz_write_int(m_fd, value);
		}
	}
	return *this;
}

Net &Net::operator<<(const char* value)
{
	std::cout << "[net] write string: " << value << std::endl;
	if(m_fd >= 0)
	{
		if(buffered)
		{
			int len = strlen(value) + 1;
			*this << len;
			memmove(queues[m_fd].send + queues[m_fd].sendpos, value, len);
			queues[m_fd].sendpos += len;
		}
		else ggz_write_string(m_fd, value);
	}
	return *this;
}

Net &Net::operator >>(int* value)
{
	if(queues[m_fd].receivepos < (int)sizeof(int))
	{
		//std::cout << "[net] warning: forced read" << std::endl;
		forceinput();
	}
	if(queues[m_fd].receivepos >= (int)sizeof(int))
	{
		memmove(value, queues[m_fd].receive, sizeof(int));
		*value = htonl(*value);
		//std::cout << "[net] read int: " << *value << std::endl;
		memmove(queues[m_fd].receive, queues[m_fd].receive + sizeof(int), queues[m_fd].receivepos - sizeof(int));
		queues[m_fd].receivepos -= sizeof(int);
	}
	else
	{
		*value = 0;
		std::cerr << "[net] error accessing empty buffer for fd " << m_fd << std::endl;
	}
	return *this;
}

Net &Net::operator >>(char** value)
{
	int len;

	*this >> &len;
	if(queues[m_fd].receivepos < len)
	{
		//std::cout << "[net] warning: forced read" << std::endl;
		forceinput();
	}
	if(queues[m_fd].receivepos >= len)
	{
		*value = (char*)ggz_malloc(len + 1);
		memmove(*value, queues[m_fd].receive, len);
		(*value)[len] = 0;
		//std::cout << "[net] read string: " << *value << std::endl;
		memmove(queues[m_fd].receive, queues[m_fd].receive + len, queues[m_fd].receivepos - len);
		queues[m_fd].receivepos -= len;
	}
	else
	{
		*value = NULL;
		std::cerr << "[net] error accessing empty buffer for fd " << m_fd << std::endl;
	}
	return *this;
}

