#include "qrecursivesortfilterproxymodel.h"

QRecursiveSortFilterProxyModel::QRecursiveSortFilterProxyModel(QObject *parent)
: QSortFilterProxyModel(parent)
{
}

bool QRecursiveSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

	if(sourceModel()->data(index).toString().contains(filterRegExp()))
		return true;

	// do children match, maybe?
	bool finalbreak = false;
	for(int row = 0; true; row++)
	{
		for(int column = 0; true; column++)
		{
			//qDebug("[modelfilter] row=%i/column=%i", row, column);
			QModelIndex childindex = index.child(row, column);
			if(!childindex.isValid())
			{
				if(column == 0)
					finalbreak = true;
				break;
			}

			if(sourceModel()->data(childindex).toString().contains(filterRegExp()))
				return true;
		}
		if(finalbreak)
			break;
	}

	// or does parent match, maybe?
	QModelIndex parentindex = index.parent();
	if(sourceModel()->data(parentindex).toString().contains(filterRegExp()))
		return true;

	// nothing matched;
	return false;
}
