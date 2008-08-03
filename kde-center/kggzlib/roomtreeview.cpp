#include "roomtreeview.h"

#include <qevent.h>

RoomTreeView::RoomTreeView(QWidget *parent)
: QTreeView(parent)
{
	setToolTip("*dummy*");
}

bool RoomTreeView::event(QEvent *event)
{
	if(event->type() == QEvent::ToolTip)
	{
		QHelpEvent *helpevent = static_cast<QHelpEvent*>(event);
		emit signalToolTip(helpevent->pos());
		return true;
	}

	return QTreeView::event(event);
}
