#ifndef SERVERLIST_H
#define SERVERLIST_H

// Qt includes
#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QMap>

class QStandardItem;
class QStandardItemModel;
class QItemSelection;
class GGZServer;

// The game window
class ServerList : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		ServerList();
		void addServer(const GGZServer& server);
		QList<GGZServer> servers();
		void clear();
		void updateServer(const GGZServer& server);

	signals:
		void signalSelected(const GGZServer& server);

	private slots:
		void slotLoaded(const QString& url, const QPixmap& pixmap);
		void slotActivated(const QItemSelection& index, const QItemSelection& prev);

	private:
		QMap<QString, QStandardItem*> m_apixmaps;
		QStandardItemModel *m_model;
		QList<GGZServer> m_servers;
};

#endif

