#ifndef KGGZ_CONNECT_H
#define KGGZ_CONNECT_H

// Qt classes
#include <qwidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

class KGGZConnect : public QWidget
{
	Q_OBJECT
	public:
		KGGZConnect(QWidget *parent, char *name);
		~KGGZConnect();
	protected slots:
		void slotAccept();
		void slotLoadProfile(int profile);
		void slotSaveProfile();
		void slotModes(int loginmode);
		void slotInvoke();
	signals:
		void signalConnect(const char *host, int port, const char *username, const char *password, int type, int server);
	private:
		int m_loginmode;
		QCheckBox *option_server;
		QComboBox *profile_select;
		QPushButton *profile_edit;
		QLineEdit *input_host, *input_port, *input_name, *input_password;
		int m_connected;
		QButtonGroup *group_mode;
};

#endif
