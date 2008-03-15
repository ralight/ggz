#ifndef PLAYER_LIST_H
#define PLAYER_LIST_H

// Qt includes
#include <QWidget>

class QStandardItemModel;
class QTreeView;
class QRecursiveSortFilterProxyModel;

// List of players
class PlayerList : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		PlayerList();
	private slots:
		void slotSearch(const QString& text);
	private:
		QStandardItemModel *m_model;
		QRecursiveSortFilterProxyModel *m_proxymodel;
		QTreeView *m_treeview;
};

#endif

