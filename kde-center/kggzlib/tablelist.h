#ifndef TABLELIST_H
#define TABLELIST_H

// Qt includes
#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QMap>

#include <kggzcore/player.h>

#include "kggzlib_export.h"

class QStandardItem;
class QStandardItemModel;
class QItemSelection;

/*namespace KGGZCore
{
	class Player;
}*/

class KGGZLIB_EXPORT TableList : public QWidget
{
	Q_OBJECT
	public:
		TableList();
		void addConfiguration(QList<KGGZCore::Player> seats);
		QList<KGGZCore::Player> configuration();
		void clear();
		void updateConfiguration(QList<KGGZCore::Player> seats, int pos);
		void removeConfiguration(int pos);

	private slots:
		//void slotLoaded(const QString& url, const QPixmap& pixmap);
		void slotActivated(const QItemSelection& index, const QItemSelection& prev);

	private:
		//QMap<QString, QStandardItem*> m_apixmaps;
		QStandardItemModel *m_model;
		QList<QList<KGGZCore::Player> > m_seats;
		//QList<GGZProfile*> m_profptrs;
		bool m_deletionmode;
};

#endif

