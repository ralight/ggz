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

///// GGZ Player /////////////////////////////////////////////////

#ifndef GGZPLAYER_H
#define GGZPLAYER_H

#include <ggzcore.h>

/**
* Player representation.
*/

class GGZCorePlayer
{
	public:
		/**
		 * Constructor */
		GGZCorePlayer(GGZPlayer *player);
		/**
		 * Destructor */
		~GGZCorePlayer();

		/**
		 * Available player types */
		enum PlayerTypes
		{
			unknown = GGZ_PLAYER_UNKNOWN,
			normal = GGZ_PLAYER_NORMAL,
			guest = GGZ_PLAYER_GUEST,
			admin = GGZ_PLAYER_ADMIN,
			host = GGZ_PLAYER_HOST,
			bot = GGZ_PLAYER_BOT
		};

		/**
		 * Available permissions */
		enum PlayerPermission
		{
			jointable = GGZ_PERM_JOIN_TABLE,
			launchtable = GGZ_PERM_LAUNCH_TABLE,
			roomslogin = GGZ_PERM_ROOMS_LOGIN,
			roomsadmin = GGZ_PERM_ROOMS_ADMIN,
			chatannounce = GGZ_PERM_CHAT_ANNOUNCE,
			chatbot = GGZ_PERM_CHAT_BOT,
			nostats = GGZ_PERM_NO_STATS,
			edittables = GGZ_PERM_EDIT_TABLES,
			tablemessage = GGZ_PERM_TABLE_PRIVMSG
		};

		/**
		 * Return the name of this player. */
		char* name();
		/**
		 * Return the player's current table. */
		GGZTable* table();
		/**
		 * Return the player's type */
		int type();

		/**
		 * Lag measurement */
		int lag();

		/**
		 * Player statistics */
		bool hasRecord();
		bool hasRating();
		bool hasRanking();
		bool hasHighscore();

		/**
		 * More player statistics */
		int recordWins();
		int recordLosses();
		int recordTies();
		int recordForfeits();
		int rating();
		int ranking();
		int highscore();

		/**
		 * Permissions */
		bool hasPermission(PlayerPermission permission);
		bool grantPermission(PlayerPermission permission);
		bool revokePermission(PlayerPermission permission);

	private:
		GGZPlayer *m_player;
};

#endif
