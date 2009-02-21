#ifndef ROOM_LIST_H
#define ROOM_LIST_H

// Qt includes
#include <QWidget>
#include <QMap>
#include <QByteArray>

#include <kggzlib/ggzprofile.h>

#include "kggzlib_export.h"

class QStandardItemModel;
class QStandardItem;
class QRecursiveSortFilterProxyModel;
class QModelIndex;
class RoomTreeView;
class Room;
class LokaRest;

// List of rooms
class KGGZLIB_EXPORT RoomList : public QWidget
{
	Q_OBJECT
	public:
		// Constructor
		RoomList();
		void setGGZProfile(const GGZProfile& ggzprofile);
		void addRoom(Room *room);
	signals:
		void signalSelected(const QString& name);
		void signalFavourite(const QString& roomname, bool favourite);
	private slots:
		void slotSearch(const QString& text);
		void slotSelected(const QPoint& pos);
		void slotSelectedLeft(const QModelIndex& index);
		void slotFavourites();
	private:
		QByteArray roomtoxml(QString roomname);

		QStandardItemModel *m_model;
		QRecursiveSortFilterProxyModel *m_proxymodel;
		RoomTreeView *m_treeview;
		QStandardItem *m_itemgame, *m_itemchat;
		QMap<QString, Room*> m_rooms;
		QString m_action_name;
		GGZProfile m_ggzprofile;
		LokaRest *m_interactor;
};

#endif

