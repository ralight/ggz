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

///// GGZ Server /////////////////////////////////////////////////

#ifndef GGZSERVER_H
#define GGZSERVER_H

#include <ggzcore.h>
#include "GGZCoreRoom.h"

/**
* A GGZCoreServer object represents not only a physical server, but also
* the connection to it. It has also functionality for room and table
* handling.
*/

class GGZCoreServer
{
	public:
		/**
		 * Possible server events */
		enum GGZCoreServerEvent
		{
			connected,
			connectfail,
			negotiated,
			negotiatefail,
			loggedin,
			loginfail,
			motdloaded,
			roomlist,
			typelist,
			entered,
			enterfail,
			loggedout,
			neterror,
			protoerror,
			chatfail,
			statechange
		};

		/**
		 * Login type */
		enum GGZCoreServerLogin
		{
			normal,
			guest,
			firsttime
		};

		/**
		 * Constructor */
		GGZCoreServer();
		/**
		 * Destructor */
		~GGZCoreServer();

		/**
		 * Add a simple callback to the server. */
		int addHook(const GGZCoreServerEvent event, const GGZHookFunc func);
		/**
		 * Add a callback with arguments. */
		int addHook(const GGZCoreServerEvent event, const GGZHookFunc func, void* data);

		/**
		 * Remove a callback from the server. */
		int removeHook(const GGZCoreServerEvent event, const GGZHookFunc func);
		/**
		 * Overloaded: Remove a callback on its id. */
		int removeHook(const GGZCoreServerEvent event, const unsigned int id);

		/**
		 * Specify the host data. */
		int setHost(const char* host, const unsigned int port, const int encryption);
		/**
		 * Specify the login information. */
		int setLogin(const int type, const char* username, const char* password);

		/**
		 * Return the hostname of this server. */
		char* host();
		/**
		 * Return the port number. */
		int port();
		/**
		 * Return the login type. */
		GGZLoginType type();
		/**
		 * Return the username with which the player is logged in. */
		char* username();
		/**
		 * Return the player's password. */
		char* password();
		/**
		 * Return the server's specific file descriptor. */
		int fd();
		/**
		 * Return the current server state. */
		GGZStateID state();
		/**
		 * Return the encryption status. */
		int encryption();

		/**
		 * The number of available rooms. */
		int countRooms();
		/**
		 * Same as above, but result is guaranteed. */
		int countRoomsForce();
		/**
		 * Return the current room. */
		GGZCoreRoom* room();
		/**
		 * Return an explicitely given room. */
		GGZCoreRoom* room(const unsigned int number);
		/**
		 * Join a room. */
		int joinRoom(const unsigned int number);

		/**
		 * The number of available game types. */
		int countGames();
		/**
		 * Return a specific game. */
		GGZGameType* game(const unsigned int number);

		/**
		 * Check whether player is online or not. */
		int isOnline();
		/**
		 * Check whether player is logged in. */
		int isLoggedIn();
		/**
		 * Check whether player is in a room or not. */
		int isInRoom();
		/**
		 * Check whether player is at a table. */
		int isAtTable();

		/**
		 * Connect to a server. This method uses the information given in @ref setHost. */
		int connect();
		/**
		 * Log into a server. See @ref setLogin for information how to set the login data. */
		int login();
		/**
		 * Invoke the message of the day. */
		int motd();
		/**
		 * Lougout of a server. */
		int logout();
		/**
		 * Cut the connection to a server. */
		int disconnect();

		/**
		 * Retrieve rooms list. */
		int listRooms(const int type, const char verbose);
		/**
		 * Retrieve games list. */
		int listGames(const char verbose);

		/**
		 * Control data flow. */
		int dataPending();
		/**
		 * Returns whether data is read. */
		int dataRead();
		/**
		 * Return whether data is written. */
		int dataWrite();

		/**
		 * Log an XML session to a file */
		int logSession(const char *filename);

		/**
		 * Return a reference to itself (servers are singleton objects for now). */
		static GGZCoreServer* instance();

		/**
		 * Retreive the associated internal ggzcore object. */
		GGZServer *server();

		/**
		 * Reset the current room. */
		void resetRoom();

		/**
		 * Safe shutdown during disconnection. */
		void rescue();

	private:
		GGZServer *m_server;
		GGZCoreRoom *m_coreroom;
		GGZRoom *m_room, *m_tmproom;
		static GGZCoreServer* m_instance;
		int m_gametypelist, m_roomlist;
};

#endif

