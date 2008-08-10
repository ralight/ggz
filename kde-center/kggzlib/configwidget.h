#ifndef PLASMA_KGGZ_CONFIGWIDGET_HEADER
#define PLASMA_KGGZ_CONFIGWIDGET_HEADER

#include <qwidget.h>

#include <kggzlib/ggzprofile.h>

#include "kggzlib_export.h"

class QLineEdit;
class QPushButton;
class QComboBox;

class KGGZLIB_EXPORT ConfigWidget : public QWidget
{
	Q_OBJECT

	public:
		ConfigWidget(QWidget *parent, bool servereditable);
		~ConfigWidget();

		void setMetaserver(const QString &metaserver);

		void setGGZProfile(const GGZProfile &profile);
		GGZProfile ggzProfile();

	signals:
		void signalChanged(const GGZProfile &profile);

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
		GGZProfile m_profile;
		QPushButton *m_roombutton;
		QString m_metaserver;
};

#endif
