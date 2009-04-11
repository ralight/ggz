#ifndef Q_RECURSIVESORTFILTERPROXYMODEL_H
#define Q_RECURSIVESORTFILTERPROXYMODEL_H

#include <qsortfilterproxymodel.h>

#include "kggzlib_export.h"

// This is like QSortFilterProxyModel
// however, when searching for models, it also honours parent items whose children match

class KGGZLIB_EXPORT QRecursiveSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
	public:
		QRecursiveSortFilterProxyModel(QObject *parent = 0);
	protected:
		bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
	private:
		bool filterAcceptsCell(const QModelIndex &index) const;
};

#endif
