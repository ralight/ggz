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

///// GGZ Module /////////////////////////////////////////////////

#ifndef GGZMODULE_H
#define GGZMODULE_H

#include <ggzcore.h>

/**
*
* A certain game module and its actual configuration can be handled by GGZCoreModule.
* This class should be used for launching games.
*
*/

class GGZCoreModule
{
	public:
		/**
		 * Environments */
		enum GGZCoreModuleEnvironment
		{
			passive = GGZ_ENVIRONMENT_PASSIVE,
			console = GGZ_ENVIRONMENT_CONSOLE,
			framebuffer = GGZ_ENVIRONMENT_FRAMEBUFFER,
			xwindow = GGZ_ENVIRONMENT_XWINDOW,
			xfullscreen = GGZ_ENVIRONMENT_XFULLSCREEN
		};

		/**
		 * Constructor */
		GGZCoreModule();
		/**
		 * Destructor */
		~GGZCoreModule();

		/**
		 * Return all available games. */
		unsigned int countAll();
		/**
		 * Return all games which match the given type. */
		unsigned int count();

		/**
		 * Add a game. */
		int add(const char* game, const char* version, const char* protocol, const char *engine,
			const char* author, const char* frontend, const char* url,
			const char* exe_path, const char* icon_path, const char* help_path,
			GGZModuleEnvironment env);

		/**
		 * Launch this module. */
		int launch();

		/**
		 * Return the name of this game module. */
		const char* game();
		/**
		 * Return the version of this game module. */
		const char* version();
		/**
		 * Return the protocol version of this game module. */
		const char* protocolVersion();
		/**
		 * Return the associated protocol engine. */
		const char *protocolEngine();
		/**
		 * Return the author of this game module. */
		const char* author();
		/**
		 * Return the frontend type of this game module. */
		const char* frontend();
		/**
		 * Return the homepage of this game module. */
		const char* url();
		/**
		 * Return the execution path of this game module, including command line options. */
		char** argv();
		/**
		 * Return the icon path of this game module. */
		const char* pathIcon();
		/**
		 * Return the help path of this game module. */
		const char* pathHelp();
		/**
		 * Return the preferred environment of this game module. */
		GGZModuleEnvironment environment();

		/**
		 * Initialize this module to find out compatible ones. */
		void init(const char* game, const char* protocol, const char *engine);
		/**
		 * Select a module as the active one. */
		void setActive(const unsigned int number);

		/**
		 * Retreive the internal ggzcore module object. */
		GGZModule *module();

	private:
		GGZModule* m_module;
		char *m_game;
		char *m_protocol;
		char *m_engine;
};

#endif

