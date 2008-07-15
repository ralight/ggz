///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001 - 2004 Josef Spillner
// josef@ggzgamingzone.org
// http://www.ggzgamingzone.org/gameclients/kdots/
//
///////////////////////////////////////////////////////////////

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
