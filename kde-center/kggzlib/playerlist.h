#ifndef PLAYER_LIST_H
#define PLAYER_LIST_H

// Qt includes
#include <QWidget>
#include <QMap>
#include <QList>
#include <QByteArray>

#include <kggzlib/ggzprofile.h>

#include "kggzlib_export.h"

class QStandardItemModel;
class QStandardItem;
class QTreeView;
class QRecursiveSortFilterProxyModel;
class Player;
class QModelIndex;
class LokaRest;

// List of players
class KGGZLIB_EXPORT PlayerList : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		PlayerList();
		void addPlayer(Player *player);
		void setGGZProfile(const GGZProfile& ggzprofile);
		void setSelf(const QString& name);
	private slots:
		void slotSearch(const QString& text);
		void slotSelected(const QPoint& index);
	private:
		void mount(QList<QStandardItem*> childitems, Player *player);
		QByteArray playertoxml(Player *player);

		QStandardItemModel *m_model;
		QRecursiveSortFilterProxyModel *m_proxymodel;
		QTreeView *m_treeview;
		QStandardItem *m_itemself, *m_itemfriends, *m_itemignored, *m_itemothers;
		QMap<QString, Player*> m_players;
		GGZProfile m_ggzprofile;
		LokaRest *m_interactor;
		QString m_self;
};

#endif

