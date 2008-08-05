#ifndef CONNECTION_PROFILES_H
#define CONNECTION_PROFILES_H

// Qt includes
#include <QWidget>

#include "ggzserver.h"

class ServerList;

// The game window
class ConnectionProfiles : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		ConnectionProfiles();
		void addServer(const GGZServer& server);

		void setMetaserver(const QString &metaserver);

	private slots:
		void slotUpdate();

	private:
		ServerList *m_serverlist;
		QString m_metaserver;
};

#endif

