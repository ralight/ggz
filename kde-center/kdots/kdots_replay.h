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
#include <QSlider>
#include <QLabel>

class QDots;
class QListWidget;

class KDotsReplay : public QWidget
{
	Q_OBJECT
	public:
		KDotsReplay();
		~KDotsReplay();
	public Q_SLOTS:
		void slotPosition(int value);
		void slotLoad(const QString& filename);
	private:
		QSlider *slider;
		QLabel *label;
		QListWidget *listbox;
		QDots *dots;
};

#endif
