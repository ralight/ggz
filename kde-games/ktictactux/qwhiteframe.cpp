//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// Includes
#include "qwhiteframe.h"

// Constructor which defines a style
QWhiteFrame::QWhiteFrame(QWidget *parent = NULL, char *name = NULL)
: QFrame(parent, name)
{
	setBackgroundColor(QColor(255.0, 255.0, 255.0));
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

// Destructor
QWhiteFrame::~QWhiteFrame()
{
}

// Return self-pointer when user clicks with a mouse
void QWhiteFrame::mousePressEvent(QMouseEvent *event)
{
	emit signalSelected(this);
}

