/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#ifndef KDOTS_REPLAY_H
#define KDOTS_REPLAY_H

#include <qwidget.h>
#include <qslider.h>
#include <qlabel.h>

class QDots;
class QListWidget;
class QListWidgetItem;

class KDotsReplay : public QWidget
{
	Q_OBJECT
	public:
		KDotsReplay();
		~KDotsReplay();
		void replay(QString filename);
	public Q_SLOTS:
		void slotPosition(int value);
		void slotLoad(QListWidgetItem *item);
	private:
		QSlider *slider;
		QLabel *label;
		QListWidget *listbox;
		QDots *dots;
};

#endif
