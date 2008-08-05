#ifndef SERVERLIST_H
#define SERVERLIST_H

// Qt includes
#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QMap>

#include "ggzserver.h"

class QStandardItem;
class QStandardItemModel;

// The game window
class ServerList : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		ServerList();
		void addServer(const GGZServer& server);

	public slots:
		void selected(const GGZServer& server);

	signals:
		void signalSelected(const GGZServer& server);

	private slots:
		void slotLoaded(const QString& url, const QPixmap& pixmap);

	private:
		QMap<QString, QStandardItem*> m_apixmaps;
		QStandardItemModel *m_model;
};

#endif

