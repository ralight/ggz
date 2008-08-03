#ifndef CONNECTION_DIALOG_H
#define CONNECTION_DIALOG_H

// Qt includes
#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QMap>

#include "ggzserver.h"

class QStandardItem;
class QStandardItemModel;

// The game window
class ConnectionDialog : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		ConnectionDialog();
		void addServer(const GGZServer& server);

	private slots:
		void slotLoaded(const QString& url, const QPixmap& pixmap);

	private:
		QMap<QString, QStandardItem*> m_apixmaps;
		QStandardItemModel *m_model;
};

#endif

