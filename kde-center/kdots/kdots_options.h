///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#ifndef KDOTS_OPTIONS_H
#define KDOTS_OPTIONS_H

#include <qwidget.h>
#include <QSlider>
#include <QLabel>

class KDotsOptions : public QWidget
{
	Q_OBJECT
	public:
		KDotsOptions();
		~KDotsOptions();
	public Q_SLOTS:
		void slotAccepted();
		void slotValueHorizontal(int value);
		void slotValueVertical(int value);
	Q_SIGNALS:
		void signalAccepted(int horizontal, int vertical);
	private:
		QSlider *sliderh, *sliderv;
		QLabel *labelhn, *labelvn;
};

#endif
