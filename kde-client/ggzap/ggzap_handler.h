#ifndef GGZAP_HANDLER_H
#define GGZAP_HANDLER_H

#include <qobject.h>

#include "GGZCoreServer.h"
#include "GGZCoreRoom.h"
#include "GGZCoreGame.h"

class GGZapHandler : public QObject
{
	Q_OBJECT
	public:
		GGZapHandler();
		~GGZapHandler();

		void init();
		void setModule(const char *modulename);
		void setFrontend(const char *frontendtype);
		void process();
		void shutdown();

		enum States
		{
			connected,
			connectfail,
			loggedin,
			loginfail,
			joinedroom,
			joinroomfail,
			waiting,
			started,
			startfail
		};

		static GGZHookReturn hookServer(unsigned int id, void *event_data, void *user_data);
		static GGZHookReturn hookRoom(unsigned int id, void *event_data, void *user_data);
		static GGZHookReturn hookGame(unsigned int id, void *event_data, void *user_data);

		void hookServerActive(unsigned int id);
		void hookRoomActive(unsigned int id);
		void hookGameActive(unsigned int id, void *data);

	signals:
		void signalState(int state);

	private:
		void attachServerCallbacks();
		void detachServerCallbacks();
		void attachRoomCallbacks();
		void detachRoomCallbacks();
		void attachGameCallbacks();
		void detachGameCallbacks();
		
		GGZCoreServer *m_server;
		GGZCoreRoom *m_room;
		GGZCoreGame *m_game;
		const char *m_modulename;
		const char *m_frontendtype;
};

#endif

