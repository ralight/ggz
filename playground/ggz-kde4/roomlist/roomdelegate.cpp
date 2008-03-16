#include "roomdelegate.h"
#include "room.h"
#include "modelview.h"

#include <qpainter.h>
#include <qabstractitemmodel.h>

RoomDelegate::RoomDelegate(QObject *parent)
: QItemDelegate(parent)
{
}

void RoomDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	void *roomptr = index.model()->data(index, ROOM_ROLE).value<void*>();
	Room *room = static_cast<Room*>(roomptr);

	QRect r = option.rect;
	int textx = 0;
	int texty = 0;

	painter->save();
	painter->setClipRect(option.rect);

	if (option.state & QStyle::State_Selected)
		painter->fillRect(option.rect, option.palette.highlight());

	// FIXME: should be better use icon.paint() here?
	// FIXME: and don't we want to get the item directly?
	QVariant variant = index.model()->data(index, Qt::DecorationRole);
	if(variant.isValid())
	{
		QIcon icon = variant.value<QIcon>();
		QPixmap pix = icon.pixmap(QSize(r.height(), r.height()));
		painter->drawPixmap(QRect(r.x(), r.y(), r.height(), r.height()), pix);
		textx = r.height() + 5;
		texty -= 10;
	}

	QString name = index.model()->data(index, Qt::DisplayRole).toString();
	QRect trect(r.x() + textx, r.y(), r.width() - textx, r.height());
	QRect bbox = painter->boundingRect(trect, 0, name);

	if(index.column() == 0)
	{
		//if(index.parent().isValid())
		if(room)
		{
			painter->setPen(QColor(70, 70, 70));
			QFont font = painter->font();
			font.setBold(true);
			font.setPointSize(font.pointSize() + 2);
			painter->setFont(font);
		}

		QRect bbtrect(trect.x(),
			trect.y() + (trect.height() - bbox.height()) / 2 + texty,
			bbox.width(), bbox.height());
		painter->drawText(bbtrect, name);
	}

	// Left column
	if(index.column() == 0)
	{
		//if(index.parent().isValid())
		if(room)
		{
			QFont font = painter->font();
			font.setBold(false);
			font.setItalic(true);
			font.setPointSize(font.pointSize() - 2);
			painter->setFont(font);

			QString roominfo = room->description();
			if(!roominfo.isEmpty())
			{
				QRect bboxinfo = painter->boundingRect(trect, 0, roominfo);
				QRect bbtrectinfo(trect.x(),
					trect.y() + trect.height() / 2 + bbox.height() + texty,
					bboxinfo.width(), bboxinfo.height());
				painter->drawText(bbtrectinfo, roominfo);
			}
		}
	}

	// Right column
	if(index.column() == 1)
	{
		if(room)
		{
			QString players = QString("%1 players").arg(room->players());
			QRect bboxplayers = painter->boundingRect(trect, 0, players);
			QRect bbtrectplayers(trect.x(),
				trect.y() + (trect.height() - bbox.height()) / 2,
				bboxplayers.width(), bboxplayers.height());
			painter->drawText(bbtrectplayers, players);

			if(room->favourite())
			{
				QPixmap pix = QPixmap("icons/rating.png");
				painter->drawPixmap(QRect(trect.x(), trect.y(), pix.height(), pix.height()), pix);
			}

			if(room->access() == Room::Locked)
			{
				QPixmap pix = QPixmap("icons/dialog-cancel.png");
				painter->drawPixmap(QRect(trect.x() + 16, trect.y(), pix.height(), pix.height()), pix);
			}
		}
	}

	painter->restore();
}

QSize RoomDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(option);

	int width = 128;
	int height = 64;

	if(!index.parent().isValid())
		height = 32;

	if(index.column() == 0)
		width = 160;

	return QSize(width, height);
}
