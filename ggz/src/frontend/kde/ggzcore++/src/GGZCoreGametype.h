/////////////////////////////////////////////////////////////////////
//                                                                 ///
// GGZCore++ - C++ API wrapper for the ggzcore library              ///
// (C) 2001 Josef Spillner                                           ///
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
		/** Constructor */
		GGZCoreGametype();
		/** Overloaded. */
		GGZCoreGametype(GGZGameType *gametype);
		/** Destructor */
		~GGZCoreGametype();

		/** The unique name of this game type. */
		char* name();
		/** Protocol version */
		char* protocolVersion();
		/** Protocol engine */
		char *protocolEngine();
		/** Game version */
		char* version();
		/** Author of this game. */
		char* author();
		/** Url to the game homepage*/
		char* url();
		/** Short description on the game. */
		char* description();

		/** Return the maximum number of players. */
		int maxPlayers();
		/** Return the maximum number of bots. */
		int maxBots();
		/** Check whether given number of players is allowed. */
		int isPlayersValid(unsigned int number);
		/** Check whether given number of bots is allowed. */
		int isBotsValid(unsigned int number);

		GGZGameType *gametype();

	private:
		GGZGameType *m_gametype;
};

#endif
