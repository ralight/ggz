#ifndef GGZAP_HANDLER_H
#define GGZAP_HANDLER_H

#include <qobject.h>

#include "GGZCoreServer.h"
#include "GGZCoreRoom.h"

class GGZapHandler : public QObject
{
	Q_OBJECT
	public:
		GGZapHandler();
		~GGZapHandler();

		void init(const char *modulename);

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
		void hook(unsigned int id);

	signals:
		void signalState(int state);

	private:
		void attachServerCallbacks();
		void detachServerCallbacks();
		
		GGZCoreServer *m_server;
		GGZCoreRoom *m_room;
		const char *m_modulename;
};

#endif

