#ifndef ROOM_TREEVIEW_H
#define ROOM_TREEVIEW_H

#include <qtreeview.h>

#include "kggzlib_export.h"

class KGGZLIB_EXPORT RoomTreeView : public QTreeView
{
	Q_OBJECT
	public:
		RoomTreeView(QWidget *parent = 0);
	signals:
		void signalToolTip(QPoint pos);
	protected:
		bool event(QEvent *event);
};

#endif
