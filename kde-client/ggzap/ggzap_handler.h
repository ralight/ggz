// GGZap - GGZ quick start application for the KDE panel
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef GGZAP_HANDLER_H
#define GGZAP_HANDLER_H

// Qt includes
#include <qobject.h>

// GGZCore++ includes
#include "GGZCoreServer.h"
#include "GGZCoreRoom.h"
#include "GGZCoreGame.h"
#include "GGZCoreModule.h"

class QSocketNotifier;

// Network connection manager for GGZap
class GGZapHandler : public QObject
{
	Q_OBJECT
	public:
		GGZapHandler();
		~GGZapHandler();

		int init();
		void setModule(const char *modulename);
		void setFrontend(const char *frontendtype);
		/*void process();*/
		void shutdown();

		enum States
		{
			connected,
			connectfail,
			negotiatefail,
			loggedin,
			loginfail,
			joinedroom,
			joinroomfail,
			waiting,
			started,
			startfail,
			finish,
			error
		};

		enum Errors
		{
			error_none,
			error_module,
			error_username
		};

		static GGZHookReturn hookServer(unsigned int id, void *event_data, void *user_data);
		static GGZHookReturn hookRoom(unsigned int id, void *event_data, void *user_data);
		static GGZHookReturn hookGame(unsigned int id, void *event_data, void *user_data);

		void hookServerActive(unsigned int id);
		void hookRoomActive(unsigned int id, void *data);
		void hookGameActive(unsigned int id, void *data);

	public slots:
		void slotServerData();
		void slotGameData();

	signals:
		void signalState(int state);

	private:
		void attachServerCallbacks();
		void detachServerCallbacks();
		void attachRoomCallbacks();
		void detachRoomCallbacks();
		void attachGameCallbacks();
		void detachGameCallbacks();

		void getModule();
		
		GGZCoreServer *m_server;
		GGZCoreRoom *m_room;
		GGZCoreGame *m_game;
		GGZCoreModule *m_module;
		const char *m_modulename;
		const char *m_frontendtype;
		const char *m_confserver, *m_confusername;
		char *m_zapuser;
		int /*m_killserver,*/ m_activetable;
		QSocketNotifier *m_sn_server, *m_sn_game;
};

#endif

