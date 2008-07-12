//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// Includes
#include "qwhiteframe.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QMouseEvent>

// Constructor which defines a style
QWhiteFrame::QWhiteFrame(int id, QWidget *parent, const char *name)
: Q3Frame(parent, name)
{
	m_id = id;
	setPaletteBackgroundColor(QColor(255, 255, 255));
	setFrameStyle(Q3Frame::Panel | Q3Frame::Sunken);
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

