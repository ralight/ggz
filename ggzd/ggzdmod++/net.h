// Networking class for ggzdmod++ library
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

#ifndef GGZDMODPP_NET_H
#define GGZDMODPP_NET_H

#if defined __GNUC__ && (__GNUC__ >= 3)
#include <ext/hash_map>
#else
#include <hash_map>
#endif

/* Internally used per-connection buffer class */
class Queue
{
	public:
		Queue(){sendpos = 0; receivepos = 0;}
		char send[1024];
		char receive[1024];
		int sendpos;
		int receivepos;
};

/* Base networking class
 * Usually, a file descriptor is assigned first, and then the data is
 * read and written.
 * *net << Net::channel << fd;
 * *net << Net::begin << 42 << Net::end << Net::flush;
 * *net << Net::input;
 * *net >> mynumber;
 */
class Net
{
	public:
		Net();
		~Net();

		/* If these opcodes are injected, the object behaviour changes:
		 * begin - start a buffered write operation
		 * end - end a buffered write operation (but do not flush)
		 * flush - flush the output buffer (also implicitly if buffer is full)
		 * channel - assign a file descriptor, which must follow as integer
		 * input - explicit reading of more bytes from network layer
		 */
		enum SpecialOpcodes
		{
			begin = -1001,
			end = -1002,
			flush = -1003,
			channel = -1004,
			input = -1005
		};

		/* Inject one of the special opcodes declared above */
		Net &operator<<(SpecialOpcodes value);
		/* Inject an integer value */
		Net &operator<<(int value);
		/* Inject a string */
		Net &operator<<(const char* value);

		/* Read an integer value */
		Net &operator>>(int* value);
		/* Read a string */
		Net &operator>>(char** value);

		/* Returns 1 if read buffer is empty, 0 if it contains data */
		int empty();

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

