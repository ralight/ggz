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

///// GGZ Table //////////////////////////////////////////////////

#ifndef GGZTABLE_H
#define GGZTABLE_H

#include <ggzcore.h>
#include <GGZCoreCommon.h>
#include <GGZCoreGametype.h>

/**
* Table representation.
*/

class GGZCoreTable
{
	public:
		/** Constructor */
		GGZCoreTable();
		/** Constructor (overloaded) */
		GGZCoreTable(GGZTable *table);
		/** Destructor */
		~GGZCoreTable();

		/** Return the name of this player. */
		//char* name();
		/** Return the player's current table. */
		//GGZCoreTable* table();

		/** Initialize a table. Set a game type and the maximum number of seats. */
		int init(GGZGameType* gametype, char* description, const unsigned int seats);

		/** Add a player to the table. */
		int addPlayer(char* name, const unsigned int seat);
		/** Add a bot to the table. */
		int addBot(char* name, const unsigned int seat);
		/** Add a reserved seat. This one can then be accessed by other players. */
		int addReserved(char* name, const unsigned int seat);
		/** Remove a player from the table. */
		int removePlayer(char* name);

		/** Return the id of the table. */
		int id();
		/** Return the game type. */
		GGZCoreGametype* type();
		/** Return the description for this table. */
		char* description();
		/** Return the current table state. */
		char state();

		/** Get the number of seats in total. */
		int countSeats();
		/** Get the number of seats occupied by bots. */
		int countBots();
		/** Get the number of open seats. If none are open, the game starts. */
		int countOpen();

		/** Return the name of a player. */
		char* playerName(const unsigned int number);
		/** Return the type of a player. */
		GGZSeatType playerType(const unsigned int number);

		GGZTable *table();

	private:
		GGZTable *m_table;
		GGZCoreGametype *m_coregametype;
		GGZGameType *m_gametype, *m_tmpgametype;
		int m_destructive;
};

#endif
