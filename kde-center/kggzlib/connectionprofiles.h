#ifndef CONNECTION_PROFILES_H
#define CONNECTION_PROFILES_H

// Qt includes
#include <QDialog>

#include "ggzserver.h"

class ServerList;
class ConfigWidget;

// The game window
class ConnectionProfiles : public QDialog
{
	Q_OBJECT
	public:
		// Constructor
		ConnectionProfiles(QWidget *parent = NULL);
		void addServer(const GGZServer& server);

		void setMetaserver(const QString &metaserver);

		QList<GGZServer> profiles();

	private slots:
		void slotUpdate();
		void slotSelected(const GGZServer& server);

	private:
		ServerList *m_serverlist;
		QString m_metaserver;
		ConfigWidget *m_configwidget;
};

#endif

