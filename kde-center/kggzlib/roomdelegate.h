#ifndef ROOM_DELEGATE_H
#define ROOM_DELEGATE_H

#include <qitemdelegate.h>

class RoomDelegate : public QItemDelegate
{
	Q_OBJECT
	public:
		RoomDelegate(QWidget *parent = 0);
		virtual void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	public slots:
		void slotToolTip(QPoint pos);
};

#endif
