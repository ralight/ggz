/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

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
