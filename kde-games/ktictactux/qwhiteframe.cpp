#include "qwhiteframe.h"

QWhiteFrame::QWhiteFrame(QWidget *parent = NULL, char *name = NULL)
: QFrame(parent, name)
{
	setBackgroundColor(QColor(255.0, 255.0, 255.0));
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

QWhiteFrame::~QWhiteFrame()
{
}

void QWhiteFrame::mousePressEvent(QMouseEvent *event)
{
	emit signalSelected(this);
}
