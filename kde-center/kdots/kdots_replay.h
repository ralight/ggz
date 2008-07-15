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
#include <qslider.h>
#include <qlabel.h>
#include <qlistbox.h>

class QDots;

class KDotsReplay : public QWidget
{
	Q_OBJECT
	public:
		KDotsReplay(QWidget *parent = NULL, const char *name = NULL);
		~KDotsReplay();
	public slots:
		void slotPosition(int value);
		void slotLoad(const QString& filename);
	private:
		QSlider *slider;
		QLabel *label;
		QListBox *listbox;
		QDots *dots;
};

#endif
