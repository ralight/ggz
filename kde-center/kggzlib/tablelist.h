#ifndef TABLELIST_H
#define TABLELIST_H

// Qt includes
#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QMap>

// KGGZ includes
#include <kggzcore/table.h>

#include "kggzlib_export.h"

class QStandardItem;
class QStandardItemModel;
class QItemSelection;
class QModelIndex;

class KGGZLIB_EXPORT TableList : public QWidget
{
	Q_OBJECT
	public:
		TableList();
		void addConfiguration(const KGGZCore::Table& table);
		//KGGZCore::Table configuration() const;
		void updateConfiguration(const KGGZCore::Table& table, int pos);
		void removeConfiguration(int pos);
		void clear();

	signals:
		void signalSelected(const KGGZCore::Table& table, int pos);

	private slots:
		//void slotLoaded(const QString& url, const QPixmap& pixmap);
		void slotActivated(const QItemSelection& index, const QItemSelection& prev);
		void slotDetails(const QModelIndex& index);

	private:
		//QMap<QString, QStandardItem*> m_apixmaps;
		QStandardItemModel *m_model;
		bool m_deletionmode;
};

#endif

