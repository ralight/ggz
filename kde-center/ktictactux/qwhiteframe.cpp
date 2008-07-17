/////////////////////////////////////////////////////////////////////
//
// KTicTacTux: Tic-Tac-Toe game for KDE 4
// http://www.ggzgamingzone.org/gameclients/ktictactux/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

// Includes
#include "qwhiteframe.h"

#include <qevent.h>

// Constructor which defines a style
QWhiteFrame::QWhiteFrame(int id)
: QLabel()
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
	if(event->button() == Qt::LeftButton)
		emit signalSelected(this);
}

// Retrieve widget id
int QWhiteFrame::id()
{
	return m_id;
}

void QWhiteFrame::setPixmap(QPixmap pix)
{
	QLabel::setPixmap(pix);
}
