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
#include <qslider.h>
#include <qlabel.h>

class KDotsOptions : public QWidget
{
	Q_OBJECT
	public:
		KDotsOptions(QWidget *parent = NULL, const char *name = NULL);
		~KDotsOptions();
	public slots:
		void slotAccepted();
		void slotValueHorizontal(int value);
		void slotValueVertical(int value);
	signals:
		void signalAccepted(int horizontal, int vertical);
	private:
		QSlider *sliderh, *sliderv;
		QLabel *labelhn, *labelvn;
};

#endif
