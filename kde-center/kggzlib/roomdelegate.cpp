#include "roomdelegate.h"
#include "room.h"
#include "roomtreeview.h"
#include "modelview.h"

#include <qpainter.h>
#include <qabstractitemmodel.h>
#include <qtooltip.h>

RoomDelegate::RoomDelegate(QWidget *parent)
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

			bbox = painter->boundingRect(trect, 0, name);
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

			QPixmap pix;

			pix = QPixmap("icons/rating.png");
			if(!(room->favourite()))
			{
				pix = QIcon(pix).pixmap(QSize(16, 16), QIcon::Disabled);
			}
			painter->drawPixmap(QRect(trect.x(), trect.y(), pix.height(), pix.height()), pix);

			pix = QPixmap("icons/dialog-cancel.png");
			if(!(room->access() == Room::Locked))
			{
				pix = QIcon(pix).pixmap(QSize(16, 16), QIcon::Disabled);
			}
			painter->drawPixmap(QRect(trect.x() + 16, trect.y(), pix.height(), pix.height()), pix);

			pix = QPixmap("icons/tagua.png");
			if(!(room->module()))
			{
				pix = QIcon(pix).pixmap(QSize(16, 16), QIcon::Disabled);
			}
			painter->drawPixmap(QRect(trect.x() + 32, trect.y(), pix.height(), pix.height()), pix);
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
		width = 300;

	return QSize(width, height);
}

void RoomDelegate::slotToolTip(QPoint pos)
{
	RoomTreeView *treeview = dynamic_cast<RoomTreeView*>(parent());

	QModelIndex index = treeview->indexAt(pos);
	if(index.isValid())
	{
		if(index.column() == 1)
		{
			void *roomptr = index.model()->data(index, ROOM_ROLE).value<void*>();
			Room *room = static_cast<Room*>(roomptr);
			if(!room)
				return;

			// FIXME: 32 is hardcoded, might depend on style
			QRect rect = treeview->visualRect(index);
			int x = pos.x() - rect.x();
			int y = pos.y() - rect.y() - 32;

			QString text;
			if((y >= 0) && (y < 16))
			{
				if((x >= 0) && (x < 16))
				{
					if(room->favourite())
					{
						text = "Favourite room!";
					}
					else
					{
						text = "Room is not a favourite";
					}
				}
				else if((x >= 16) && (x < 32))
				{
					if(room->access() == Room::Locked)
					{
						text = "Room is locked, you cannot join.";
					}
					else
					{
						text = "Room is open for you to join.";
					}
				}
				else if((x >= 32) && (x < 48))
				{
					if(room->module())
					{
						text = "You can play a game here.";
					}
					else
					{
						text = "No suitable game clients found, only chat possible.";
					}
				}
			}

			QToolTip::showText(treeview->mapToGlobal(pos), text);
		}
	}
}
