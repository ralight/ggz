#ifndef PLAYER_LIST_H
#define PLAYER_LIST_H

// Qt includes
#include <QWidget>
#include <QMap>

class QStandardItemModel;
class QStandardItem;
class QTreeView;
class QRecursiveSortFilterProxyModel;
class Player;
class QModelIndex;

// List of players
class PlayerList : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		PlayerList();
		void addPlayer(Player *player);
	private slots:
		void slotSearch(const QString& text);
		void slotSelected(const QPoint& index);
	private:
		QStandardItemModel *m_model;
		QRecursiveSortFilterProxyModel *m_proxymodel;
		QTreeView *m_treeview;
		QStandardItem *m_itemfriends, *m_itemignored, *m_itemothers;
		QMap<QString, Player*> m_players;
};

#endif

