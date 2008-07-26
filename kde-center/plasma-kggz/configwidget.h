#ifndef PLASMA_KGGZ_CONFIGWIDGET_HEADER
#define PLASMA_KGGZ_CONFIGWIDGET_HEADER

#include <qwidget.h>

#include "ggzserver.h"

class QLineEdit;
class QPushButton;

class ConfigWidget : public QWidget
{
	Q_OBJECT

	public:
		ConfigWidget(QWidget *parent);
		~ConfigWidget();

		void setUsername(const QString &username);
		QString username() const;

		void setPassword(const QString &password);
		QString password() const;

		void setRoomname(const QString &roomname);
		QString roomname() const;

		void setGGZServer(const GGZServer &server);
		GGZServer ggzServer() const;

	private slots:
		void slotSelectRoom();
		void slotSelectServer();

	private:
		QLineEdit *m_username;
		QLineEdit *m_password;
		QLineEdit *m_roomname;
		QLineEdit *m_ggzserver;
		GGZServer m_server;
		QPushButton *m_roombutton;
};

#endif
