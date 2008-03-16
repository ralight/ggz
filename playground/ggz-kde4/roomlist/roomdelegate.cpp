#include "roomdelegate.h"

#include <qpainter.h>
#include <qabstractitemmodel.h>

RoomDelegate::RoomDelegate(QObject *parent)
: QItemDelegate(parent)
{
}

void RoomDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	/*if(index.column() == 1)
	{
		QItemDelegate::paint(painter, option, index);
		return;
	}*/

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

	if(index.column() == 0)
	{
		if(index.parent().isValid())
		{
			painter->setPen(QColor(70, 70, 70));
			QFont font = painter->font();
			font.setBold(true);
			font.setPointSize(font.pointSize() + 2);
			painter->setFont(font);
		}
	}

	QString name = index.model()->data(index, Qt::DisplayRole).toString();
	QRect trect(r.x() + textx, r.y(), r.width() - textx, r.height());
	QRect bbox = painter->boundingRect(trect, 0, name);
	QRect bbtrect(trect.x(),
		trect.y() + (trect.height() - bbox.height()) / 2 + texty,
		bbox.width(), bbox.height());
	painter->drawText(bbtrect, name);

	if(index.column() == 0)
	{
		if(index.parent().isValid())
		{
			QFont font = painter->font();
			font.setBold(false);
			font.setItalic(true);
			font.setPointSize(font.pointSize() - 2);
			painter->setFont(font);

			QString roominfo = "A room to do fun things in...";
			QRect bboxinfo = painter->boundingRect(trect, 0, roominfo);
			QRect bbtrectinfo(trect.x(),
				trect.y() + trect.height() / 2 + bbox.height() + texty,
				bboxinfo.width(), bboxinfo.height());
			painter->drawText(bbtrectinfo, roominfo);
		}
	}

	painter->restore();
}

QSize RoomDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	/*if(index.column() == 1)
		return QSize(128, 64);
	else
		return QItemDelegate::sizeHint(option, index);*/

	if(!index.parent().isValid())
		return QSize(128, 32);

	return QSize(128, 64);
}
