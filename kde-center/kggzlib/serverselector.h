#ifndef PLASMA_KGGZ_SERVERSELECTOR_HEADER
#define PLASMA_KGGZ_SERVERSELECTOR_HEADER

#include <qdialog.h>
#include <qabstractsocket.h>

#include <kggzlib/ggzserver.h>

#include "kggzlib_export.h"

class QPushButton;
class QTcpSocket;
class QProgressBar;
class GGZProfile;
class ServerList;

class KGGZLIB_EXPORT ServerSelector : public QDialog
{
	Q_OBJECT

	public:
		ServerSelector(QWidget *parent);
		~ServerSelector();

		void setMetaUri(QString uri);
		GGZServer server();

	//signals:
	//	void signalServerSelected(const GGZServer &server);

	private slots:
		void slotSelected(const GGZProfile& profile, int pos);

		void slotConnected();
		void slotError(QAbstractSocket::SocketError error);
		void slotData();

	private:
		ServerList *m_serverlist;
		QPushButton *m_button;
		QTcpSocket *m_sock;
		GGZServer m_server;
		QProgressBar *m_indicator;
};

#endif
