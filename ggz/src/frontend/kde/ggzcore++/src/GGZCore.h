/////////////////////////////////////////////////////////////////////
//                                                                 ///
// GGZCore++ - C++ API wrapper for the ggzcore library              ///
// Copyright (C) 2001, 2002 Josef Spillner                           ///
// dr_maux@users.sourceforge.net                                      ///
// The MindX Open Source Project                                      ///
// http://mindx.sourceforge.net/                                     ///
//                                                                  ///
// This library may be distributed under the terms of the GNU GPL. ///
//                                                                ///
////////////////////////////////////////////////////////////////////

///// GGZ Core ///////////////////////////////////////////////////

#ifndef GGZCORE_PP_H
#define GGZCORE_PP_H

#include <ggzcore.h>

/**
*
* This is the main class. There must be at least one GGZCore object if one wants to create other objects
* (exceptions apply to the configuration classes).
*
*/

class GGZCore
{
	public:
		/** Options use to initialize the core lib. */
		enum GGZCoreOptions
		{
			parser   = 1,
			modules  = 2,
			threadio = 4
		};

		/** Debug options. This is an or'd list. */
		enum GGZCoreDebug
		{
			event     = 0x000001,
			net       = 0x000002,
			user      = 0x000004,
			server    = 0x000008,
			conf      = 0x000010,
			poll      = 0x000020,
			state     = 0x000040,
			player    = 0x000080,
			room      = 0x000100,
			table     = 0x000200,
			gametype  = 0x000400,
			hook      = 0x000800,
			start     = 0x001000,
			memory    = 0x002000,
			memdetail = 0x004000,
			module    = 0x008000,
			game      = 0x010000,
			xml       = 0x020000,
			all       = 0xFFFFFF
		};

		/** Constructor */
		GGZCore();
		/** Destructor */
		~GGZCore();

		/** Initialize ggzcore. */
		int init(int options, const char* debugfile, int debuglevel);

	private:
		int m_initialized;
};

#endif
