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

///// GGZ GameType ///////////////////////////////////////////////

#ifndef GGZGAMETYPE_H
#define GGZGAMETYPE_H

#include <ggzcore.h>

/**
* A class which contains all information on a certain game type.
*/

class GGZCoreGametype
{
	public:
		/**
		 * Constructor */
		GGZCoreGametype();
		/**
		 * Overloaded. */
		GGZCoreGametype(const GGZGameType *gametype);
		/**
		 * Destructor */
		~GGZCoreGametype();

		/**
		 * The unique name of this game type. */
		const char* name();
		/**
		 * Protocol version */
		const char* protocolVersion();
		/**
		 * Protocol engine */
		const char *protocolEngine();
		/**
		 * Game version */
		const char* version();
		/**
		 * Author of this game. */
		const char* author();
		/**
		 * Url to the game homepage*/
		const char* url();
		/**
		 * Short description on the game. */
		const char* description();
		/**
		 * Game category */
		const char* category();
		/**
		 * Game rating */
		const char* rating();

		/**
		 * Return the maximum number of players. */
		int maxPlayers();
		/**
		 * Return the maximum number of bots. */
		int maxBots();
		/**
		 * Return the maximum number of spectators */
		int maxSpectators();
		/**
		 * Check whether given number of players is allowed. */
		int isPlayersValid(unsigned int number);
		/**
		 * Check whether given number of bots is allowed. */
		int isBotsValid(unsigned int number);
		/**
		 * Check whether a given number of spectators is allowed */
		int isSpectatorsValid(unsigned int number);

		/**
		 * Get the number of named bots */
		int namedBots();
		/**
		 * Get the name of a named bot */
		const char *namedBotName(unsigned int number);
		/**
		 * Get the class of a named bot */
		const char *namedBotClass(unsigned int number);

		/**
		 * Retrieve the wrapped ggzcore object for this game type. */
		const GGZGameType *gametype();

	private:
		const GGZGameType *m_gametype;
};

#endif

