#ifndef PLASMA_KGGZ_CONFIGWIDGET_HEADER
#define PLASMA_KGGZ_CONFIGWIDGET_HEADER

#include <qwidget.h>

#include <kggzlib/ggzserver.h>

class QLineEdit;
class QPushButton;
class QComboBox;

class ConfigWidget : public QWidget
{
	Q_OBJECT

	public:
		ConfigWidget(QWidget *parent, bool servereditable);
		~ConfigWidget();

		enum LoginType
		{
			guest,
			firsttime,
			registered
		};

		void setMetaserver(const QString &metaserver);

		void setUsername(const QString &username);
		QString username() const;

		void setPassword(const QString &password);
		QString password() const;

		void setRoomname(const QString &roomname);
		QString roomname() const;

		void setRealname(const QString &realname);
		QString realname() const;

		void setEmail(const QString &email);
		QString email() const;

		void setLoginType(LoginType type);
		LoginType loginType() const;

		void setGGZServer(const GGZServer &server);
		GGZServer ggzServer() const;

	signals:
		void signalChanged(const GGZServer &server);

	private slots:
		void slotSelectRoom();
		void slotSelectServer();
		void slotLoginType(int index);

	private:
		QLineEdit *m_username;
		QLineEdit *m_password;
		QLineEdit *m_roomname;
		QLineEdit *m_realname;
		QLineEdit *m_email;
		QComboBox *m_logintype;
		QLineEdit *m_ggzserver;
		GGZServer m_server;
		QPushButton *m_roombutton;
		QString m_metaserver;
};

#endif
