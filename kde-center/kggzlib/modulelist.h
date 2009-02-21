#ifndef MODULELIST_H
#define MODULELIST_H

// Qt includes
#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QMap>

// KGGZ includes
#include <kggzcore/module.h>

#include "kggzlib_export.h"

class QStandardItem;
class QStandardItemModel;
class QItemSelection;

class KGGZLIB_EXPORT ModuleList : public QWidget
{
	Q_OBJECT
	public:
		ModuleList();
		void addModule(const KGGZCore::Module& module);

	signals:
		void signalSelected(const KGGZCore::Module& module);

	private slots:
		void slotActivated(const QItemSelection& index, const QItemSelection& prev);

	private:
		QStandardItemModel *m_model;
};

#endif

