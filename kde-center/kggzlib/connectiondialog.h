#ifndef CONNECTION_DIALOG_H
#define CONNECTION_DIALOG_H

// Qt includes
#include <QWidget>

#include "ggzserver.h"

class ServerList;
class QPushButton;

// The game window
class ConnectionDialog : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		ConnectionDialog();
		void addServer(const GGZServer& server);

	//signals:
	//	void signalSelected(QString uri);

	private slots:
		void slotManage();
		void slotConnect();
		void slotSelected(const GGZServer& server);

	private:
		void load();

		ServerList *m_serverlist;
		QPushButton *m_connect_button;
		QString m_uri;
};

#endif

