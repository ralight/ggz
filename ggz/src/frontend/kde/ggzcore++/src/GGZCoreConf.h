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

///// GGZ Configuration //////////////////////////////////////////

#ifndef GGZCONF_H
#define GGZCONF_H

#include <ggzcore.h>

/**
*
* A GGZCoreConf object is intended to deal with configuration files.
* It is able to use sections and string lists.
*
*/

class GGZCoreConf
{
	public:
		/**
		 * Constructor. */
		GGZCoreConf();
		/**
		 * Destructor */
		~GGZCoreConf();

		/**
		 * Initialize the configuration object with the given global and local configuration path. */
		int init(const char* global, const char* local);

		/**
		 * Write a string. */
		int write(const char* section, const char* key, const char* value);
		/**
		 * Write an integer value. */
		int write(const char* section, const char* key, int value);
		/**
		 * Write a NULL-terminated list of strings. */
		int write(const char* section, const char* key, int argc, char** argv);

		/**
		 * Read a string from the configuration. Use @ref def as the default value if key not found. NULL may also be specified as the default. */
		char* read(const char* section, const char* key, const char* def);
		/**
		 * Read an integer value. */
		int read(const char* section, const char* key, int def);
		/**
		 * Read a dynamically allocated list of strings. */
		int read(const char* section, const char* key, int* argcp, char*** argvp);

		/**
		 * Remove an entry in the given section. */
		int removeKey(const char* section, const char* key);
		/**
		 * Remove a whole section. */
		int removeSection(const char* section);

		/**
		 * Commit all write changes. */
		int commit();
};

#endif

