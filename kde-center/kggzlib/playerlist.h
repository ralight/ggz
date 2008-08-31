#ifndef PLAYER_LIST_H
#define PLAYER_LIST_H

// Qt includes
#include <QWidget>
#include <QMap>
#include <QList>
#include <QByteArray>

#include <kggzlib/ggzserver.h>

#include "kggzlib_export.h"

class QStandardItemModel;
class QStandardItem;
class QTreeView;
class QRecursiveSortFilterProxyModel;
class Player;
class QModelIndex;
class WSInteractor;

// List of players
class KGGZLIB_EXPORT PlayerList : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		PlayerList();
		void addPlayer(Player *player);
		void setGGZServer(const GGZServer& ggzserver);
	private slots:
		void slotSearch(const QString& text);
		void slotSelected(const QPoint& index);
	private:
		void mount(QList<QStandardItem*> childitems, Player *player);
		QByteArray playertoxml(Player *player);

		QStandardItemModel *m_model;
		QRecursiveSortFilterProxyModel *m_proxymodel;
		QTreeView *m_treeview;
		QStandardItem *m_itemfriends, *m_itemignored, *m_itemothers;
		QMap<QString, Player*> m_players;
		GGZServer m_ggzserver;
		WSInteractor *m_interactor;
};

#endif

