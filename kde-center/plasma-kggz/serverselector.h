#ifndef PLASMA_KGGZ_SERVERSELECTOR_HEADER
#define PLASMA_KGGZ_SERVERSELECTOR_HEADER

#include <qdialog.h>
#include <qabstractsocket.h>

#include <kggzlib/ggzserver.h>

class QListWidget;
class QPushButton;
class QTcpSocket;

class ServerSelector : public QDialog
{
	Q_OBJECT

	public:
		ServerSelector(QWidget *parent);
		~ServerSelector();

		void setMetaUri(QString uri);
		GGZServer server();

	signals:
		void signalServerSelected(const GGZServer &server);

	private slots:
		void slotServerSelected();
		void slotSelectionChanged();

		void slotConnected();
		void slotError(QAbstractSocket::SocketError error);
		void slotData();

	private:
		QListWidget *m_serverlist;
		QPushButton *m_button;
		QTcpSocket *m_sock;
		QMap<QString, GGZServer> m_servers;
};

#endif
