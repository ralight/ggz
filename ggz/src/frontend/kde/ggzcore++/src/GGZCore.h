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
		/**
		 * Options use to initialize the core lib. */
		enum GGZCoreOptions
		{
			parser   = GGZ_OPT_PARSER,
			modules  = GGZ_OPT_MODULES,
			threadio = GGZ_OPT_THREADED_IO
		};
		
		/**
		 * Constructor */
		GGZCore();
		/**
		 * Destructor */
		~GGZCore();

		/**
		 * Initialize ggzcore. */
		int init(int options);

		/**
		 * Reload the game module database. */
		void reload();

	private:
		int m_initialized;
};

#endif

