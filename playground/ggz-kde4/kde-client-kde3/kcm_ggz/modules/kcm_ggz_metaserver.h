#ifndef KCM_GGZ_METASERVER_H
#define KCM_GGZ_METASERVER_H

#include "kcm_ggz_pane.h"

class KListView;
class QSocket;
class QListViewItem;

class KCMGGZMetaserver : public KCMGGZPane
{
	Q_OBJECT
	public:
		KCMGGZMetaserver(QWidget *parent = NULL, const char *name = NULL);
		~KCMGGZMetaserver();
		void load();
		void save();
		QString caption();

	public slots:
		void slotDelete();
		void slotDeleteServer();
		void slotAdd();
		void slotAddServer();
		void slotAuto();
		void slotAutoServer();
		void slotAutoConnected();
		void slotAutoRead();
		void slotAdded(QString uri, QString proto);
		void slotAddedServer(QString uri, QString type, QString location, QString speed, QString comment);
		void slotAutoError(int error);
		void slotSelected(QListViewItem *item, const QPoint& point, int column);

	signals:
		void signalChanged();

	private:
		void addURI(QString uri, QString proto);
		void addServerURI(QString uri, QString type, QString pref, QString location, QString speed, QString comment);

		KListView *view, *view_servers;
		QSocket *sock;
		enum QueryTypes
		{
			query_invalid,
			query,
			query_server
		};
		int m_query;
};

#endif

