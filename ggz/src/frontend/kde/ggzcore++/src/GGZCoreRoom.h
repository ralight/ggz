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

///// GGZ Room ///////////////////////////////////////////////////

#ifndef GGZROOM_H
#define GGZROOM_H

#include <ggzcore.h>
#include <GGZCoreCommon.h>
#include <GGZCoreGametype.h>
#include <GGZCoreTable.h>

/**
* A GGZCoreRoom stands for a room on a server, which is connected to a
* certain game type.
*/

class GGZCoreRoom
{
	public:
		enum GGZCoreRoomEvent
		{
			playerlist,
			tablelist,
			chatnormal,
			chatannounce,
			chatprivate,
			chatbeep,
			enter,
			leave,
			tableupdate,
			tablelaunched,
			tablelaunchfail,
			tablejoined,
			tablejoinfail,
			tableleft,
			tableleavefail,
			tabledata
		};

		/** Constructor */
		GGZCoreRoom();
		/** Constructor, overloaded to assign an existant room. */
		GGZCoreRoom(GGZRoom* room);
		/** Destructor */
		~GGZCoreRoom();

		/** Add a simple callback to the room. */
		int addHook(const GGZCoreRoomEvent event, const GGZHookFunc func);
		/** Add a callback with arguments. */
		int addHook(const GGZCoreRoomEvent event, const GGZHookFunc func, void* data);

		/** Remove a callback from the room. */
		int removeHook(const GGZCoreRoomEvent event, const GGZHookFunc func);
		/** Overloaded: Remove a callback on its id. */
		int removeHook(const GGZCoreRoomEvent event, const unsigned int id);

		/** Initialize a room with the required information. */
		int init(const GGZServer* server, const unsigned int id, const char* name, const unsigned int game, const char* description, const char *category);

		/** Return the name of the room. */
		char* name();
		/** Return its description. */
		char* description();
		/** Return its category */
		char* category();
		/** Return the associated game type. */
		GGZCoreGametype* gametype();

		/** Return the number of players in this room. */
		int countPlayers();
		/** Return specified player. */
		GGZPlayer* player(const unsigned int number);

		/** Count number of launched tables. */
		int countTables();
		/** Return the specified table. */
		GGZCoreTable* table(const unsigned int number);
		/** Launch a table */
		int launchTable(GGZTable* table);
		/** Join an already launched table. */
		int joinTable(const unsigned int number);
		/** Leave a table again. */
		int leaveTable();

		/** Invoke player listing. */
		int listPlayers();
		/** Invoke tables listing. */
		int listTables(const int type, const char global);

		/** Send a chat message to the other players. */
		int chat(const GGZChatOp opcode, const char* player, const char* message);
		/** Send other data. */
		int sendData(char* buffer);

		/** Return the internal GGZRoom structure */
		GGZRoom* room();

		void selfRegister(GGZCoreRoom **room);

	private:
		GGZRoom *m_room;
		GGZGameType *m_gametype, *m_tmpgametype;
		GGZCoreGametype *m_coregametype;
		GGZTable *m_table, *m_tmptable;
		GGZCoreTable *m_coretable;
		GGZCoreRoom **m_register;
		int m_destructive;
};

#endif
