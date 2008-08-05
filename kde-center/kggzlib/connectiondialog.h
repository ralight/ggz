#ifndef CONNECTION_DIALOG_H
#define CONNECTION_DIALOG_H

// Qt includes
#include <QWidget>

#include "ggzserver.h"

class ServerList;

// The game window
class ConnectionDialog : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		ConnectionDialog();
		void addServer(const GGZServer& server);

	private slots:
		void slotManage();
		void slotConnect();

	private:
		ServerList *m_serverlist;
};

#endif

