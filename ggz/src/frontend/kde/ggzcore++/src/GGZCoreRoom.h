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

class GGZCoreGametype;
class GGZCoreTable;
class GGZCorePlayer;

/**
* A GGZCoreRoom stands for a room on a server, which is connected to a
* certain game type.
*/

class GGZCoreRoom
{
	public:
		/**
		 * Possible events which can occur when entering rooms and staying in them. */
		enum GGZCoreRoomEvent
		{
			/* Note, these events are linked to the real ggzcore values since they MUST match. */
			playerlist = GGZ_PLAYER_LIST,
			tablelist = GGZ_TABLE_LIST,
			chatevent = GGZ_CHAT_EVENT,
			enter = GGZ_ROOM_ENTER,
			leave = GGZ_ROOM_LEAVE,
			tableupdate = GGZ_TABLE_UPDATE,
			tablelaunched = GGZ_TABLE_LAUNCHED,
			tablelaunchfail = GGZ_TABLE_LAUNCH_FAIL,
			tablejoined = GGZ_TABLE_JOINED,
			tablejoinfail = GGZ_TABLE_JOIN_FAIL,
			tableleft = GGZ_TABLE_LEFT,
			tableleavefail = GGZ_TABLE_LEAVE_FAIL,
			playerlag = GGZ_PLAYER_LAG
		};

		enum ChatTypes
		{
			chatnormal = GGZ_CHAT_NORMAL,
			chatannounce = GGZ_CHAT_ANNOUNCE,
			chatprivate = GGZ_CHAT_PERSONAL,
			chatbeep = GGZ_CHAT_BEEP,
			chattable = GGZ_CHAT_TABLE,
			chatunknown
		};

		/**
		 * Constructor */
		GGZCoreRoom();
		/**
		 * Constructor, overloaded to assign an existant room. */
		GGZCoreRoom(GGZRoom* room);
		/**
		 * Destructor */
		~GGZCoreRoom();

		/**
		 * Add a simple callback to the room. */
		int addHook(const GGZCoreRoomEvent event, const GGZHookFunc func);
		/**
		 * Add a callback with arguments. */
		int addHook(const GGZCoreRoomEvent event, const GGZHookFunc func, void* data);

		/**
		 * Remove a callback from the room. */
		int removeHook(const GGZCoreRoomEvent event, const GGZHookFunc func);
		/**
		 * Overloaded: Remove a callback on its id. */
		int removeHook(const GGZCoreRoomEvent event, const unsigned int id);

		/**
		 * Initialize a room with the required information. */
		int init(const GGZServer* server, const unsigned int id, const char* name, const unsigned int game, const char* description, const char *category);

		/**
		 * Return the name of the room. */
		char* name();
		/**
		 * Return its description. */
		char* description();
		/**
		 * Return its category */
		char* category();
		/**
		 * Return the associated game type. */
		GGZCoreGametype* gametype();

		/**
		 * Return the number of players in this room. */
		int countPlayers();
		/**
		 * Return specified player. */
		GGZCorePlayer* player(const unsigned int number);

		/**
		 * Count number of launched tables. */
		int countTables();
		/**
		 * Return the specified table. */
		GGZCoreTable* table(const unsigned int number);
		/**
		 * Launch a table */
		int launchTable(GGZTable* table);
		/**
		 * Join an already launched table. */
		int joinTable(const unsigned int number);
		/**
		 * Join a table as spectator. */
		int joinTableSpectator(const unsigned int number);
		/**
		 * Leave a table again. */
		int leaveTable(int force);
		/**
		 * Leave a table as spectator. */
		int leaveTableSpectator();

		/**
		 * Invoke player listing. */
		int listPlayers();
		/**
		 * Invoke tables listing. */
		int listTables(const int type, const char global);

		/**
		 * Send a chat message to the other players. */
		int chat(const GGZChatType opcode, const char* player, const char* message);
		/**
		 * Send other data. */
		int sendData(char* buffer);

		/**
		 * Return the internal GGZRoom structure */
		GGZRoom* room();

		/**
		 * Get a reference on itself. */
		void selfRegister(GGZCoreRoom **room);

	private:
		GGZRoom *m_room;
		GGZGameType *m_gametype, *m_tmpgametype;
		GGZCoreGametype *m_coregametype;
		GGZTable *m_table, *m_tmptable;
		GGZPlayer *m_player, *m_tmpplayer;
		GGZCoreTable *m_coretable;
		GGZCorePlayer *m_coreplayer;
		GGZCoreRoom **m_register;
		int m_destructive;
};

#endif

