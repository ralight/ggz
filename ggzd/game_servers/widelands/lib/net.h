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

#ifndef TUXMAN_NET_H
#define TUXMAN_NET_H

#if defined __GNUC__ && (__GNUC__ >= 3)
#include <ext/hash_map>
#else
#include <hash_map>
#endif

class Queue
{
	public:
		Queue(){sendpos = 0; receivepos = 0;}
		char send[1024];
		char receive[1024];
		int sendpos;
		int receivepos;
};

class Net
{
	public:
		Net();
		~Net();

		enum SpecialOpcodes
		{
			begin = -1001,
			end = -1002,
			flush = -1003,
			channel = -1004,
			input = -1005
		};

		Net &operator<<(SpecialOpcodes value);
		Net &operator<<(int value);
		Net &operator<<(const char* value);

		Net &operator>>(int* value);
		Net &operator>>(char** value);

	private:
		void forceinput();

		int m_fd;
		int waitforchannel;
		int buffered;
#if defined __GNUC__ && (__GNUC__ >= 3)
		__gnu_cxx::hash_map<int, Queue> queues;
#else
		hash_map<int, Queue> queues;
#endif
};

#endif

