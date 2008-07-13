//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

#ifndef Q_WHITEFRAME_H
#define Q_WHITEFRAME_H

// Qt includes
#include <qframe.h>
#include <qevent.h>
#include <QMouseEvent>

// A special Qt class
class QWhiteFrame : public QFrame
{
	Q_OBJECT
	public:
		// Constructor
		QWhiteFrame(int id);
		// Destructor
		~QWhiteFrame();
		// Retrieve widget id
		int id();
		void setPixmap(QPixmap pix);
	protected slots:
		// Receive a mouse event
		void mousePressEvent(QMouseEvent *event);
	signals:
		// Return a self-pointer
		void signalSelected(QWidget *);
	private:
		int m_id;
};

#endif

