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

///// GGZ Configuration I/O //////////////////////////////////////

#ifndef GGZCONFIO_H
#define GGZCONFIO_H

#include <ggz.h>

/**
*
* This is similar to @ref GGZCoreConf, but provides additionally a mode which applies
* to the way one uses the methods.
*
*/

class GGZCoreConfio
{
	public:
		/**
		 * Options which are passed to the constructor as an or'd list. */
		enum GGZCoreConfioOptions
		{
			readonly  = GGZ_CONF_RDONLY,
			readwrite = GGZ_CONF_RDWR,
			create    = GGZ_CONF_CREATE
		};

		/**
		 * Constructor. Its arguments specify the configuration file path and the options to operate on this file. */
		GGZCoreConfio(const char* path, const int options);
		/**
		 * Destructor */
		~GGZCoreConfio();

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

		/**
		 * Free allocated memory */
		static void free(void *ptr);
		/**
		 * Close all open configuration files */
		static void cleanup();

	private:
		int m_confio;
};

#endif

