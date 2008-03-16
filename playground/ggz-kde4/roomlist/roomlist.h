#ifndef ROOM_LIST_H
#define ROOM_LIST_H

// Qt includes
#include <QWidget>
#include <QMap>

class QStandardItemModel;
class QStandardItem;
class QTreeView;
class QRecursiveSortFilterProxyModel;
class QModelIndex;
class Room;

// List of rooms
class RoomList : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		RoomList();
		void addRoom(Room *room);
	private slots:
		void slotSearch(const QString& text);
		void slotSelected(const QPoint& index);
		void slotFavourites();
	private:
		QStandardItemModel *m_model;
		QRecursiveSortFilterProxyModel *m_proxymodel;
		QTreeView *m_treeview;
		QStandardItem *m_itemgame, *m_itemchat;
		QMap<QString, Room*> m_rooms;
		QString m_action_name;
};

#endif

