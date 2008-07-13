//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// Includes
#include "qwhiteframe.h"
#include <QFrame>
#include <QMouseEvent>

// Constructor which defines a style
QWhiteFrame::QWhiteFrame(int id)
: QFrame()
{
	m_id = id;
	setFrameStyle(QFrame::Panel | QFrame::Sunken);

	setPixmap(QPixmap());
}

// Destructor
QWhiteFrame::~QWhiteFrame()
{
}

// Return self-pointer when user clicks with a mouse
void QWhiteFrame::mousePressEvent(QMouseEvent *event)
{
	Q_UNUSED(event);

	emit signalSelected(this);
}

// Retrieve widget id
int QWhiteFrame::id()
{
	return m_id;
}

void QWhiteFrame::setPixmap(QPixmap pix)
{
	QPalette palette(QColor(255, 255, 255));
	if(!pix.isNull())
		palette.setBrush(backgroundRole(), QBrush(pix));
	setPalette(palette);
}
