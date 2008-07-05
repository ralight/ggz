#ifndef PLASMA_KGGZ_CONFIGWIDGET_HEADER
#define PLASMA_KGGZ_CONFIGWIDGET_HEADER

#include <qwidget.h>

class QLineEdit;

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

	private:
		QLineEdit *m_username;
		QLineEdit *m_password;
		QLineEdit *m_roomname;
};

#endif
