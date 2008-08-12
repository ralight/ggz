#ifndef CONNECTION_DIALOG_H
#define CONNECTION_DIALOG_H

// Qt includes
#include <QDialog>

#include "kggzlib_export.h"

class ServerList;
class QPushButton;
class GGZProfile;

// The game window
class KGGZLIB_EXPORT ConnectionDialog : public QDialog
{
	Q_OBJECT
	public:
		// Constructor
		ConnectionDialog(QWidget *parent = NULL);
		QString uri();

	private slots:
		void slotManage();
		void slotConnect();
		void slotSelected(const GGZProfile& profile, int pos);

	private:
		void load();
		void addProfile(const GGZProfile& profile);

		ServerList *m_serverlist;
		QPushButton *m_connect_button;
		QString m_uri;
};

#endif
