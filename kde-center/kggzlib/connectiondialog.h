#ifndef CONNECTION_DIALOG_H
#define CONNECTION_DIALOG_H

// Qt includes
#include <QDialog>

#include "kggzlib_export.h"

class ServerList;
class QPushButton;
class GGZProfile;
class KGGZCoreLayer;
class QProgressBar;

// The game window
class KGGZLIB_EXPORT ConnectionDialog : public QDialog
{
	Q_OBJECT
	public:
		// Constructor
		ConnectionDialog(QWidget *parent = NULL);
		void setGame(QString engine, QString version);
		QString uri();

		KGGZCoreLayer *layer() const;

		void setCore(bool core);

	private slots:
		void slotManage();
		void slotConnect();
		void slotSelected(const GGZProfile& profile, int pos);
		void slotReady(bool ready);
		void slotRoomReady(bool ready);

	private:
		void load();
		void addProfile(const GGZProfile& profile);

		ServerList *m_serverlist;
		QPushButton *m_connect_button;
		QString m_uri;
		KGGZCoreLayer *m_corelayer;
		QProgressBar *m_indicator;
		QString m_engine;
		QString m_version;
		QString m_username;

		bool m_core;
};

#endif

