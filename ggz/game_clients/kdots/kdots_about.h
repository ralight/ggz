///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#ifndef KDOTS_ABOUT_H
#define KDOTS_ABOUT_H

#include <qwidget.h>

class KDotsAbout : public QWidget
{
	Q_OBJECT
	public:
		KDotsAbout(QWidget *parent, char *name);
		~KDotsAbout();
	public slots:
		void slotAccepted();
};

#endif
