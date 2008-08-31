#ifndef PLAYER_LIST_H
#define PLAYER_LIST_H

// Qt includes
#include <QWidget>
#include <QMap>
#include <QList>

#include "kggzlib_export.h"

class QStandardItemModel;
class QStandardItem;
class QTreeView;
class QRecursiveSortFilterProxyModel;
class Player;
class QModelIndex;

// List of players
class KGGZLIB_EXPORT PlayerList : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		PlayerList();
		void addPlayer(Player *player);
		void setCommunityUrl(QString url);
	private slots:
		void slotSearch(const QString& text);
		void slotSelected(const QPoint& index);
	private:
		void mount(QList<QStandardItem*> childitems, Player *player);

		QStandardItemModel *m_model;
		QRecursiveSortFilterProxyModel *m_proxymodel;
		QTreeView *m_treeview;
		QStandardItem *m_itemfriends, *m_itemignored, *m_itemothers;
		QMap<QString, Player*> m_players;
		QString m_url;
};

#endif

