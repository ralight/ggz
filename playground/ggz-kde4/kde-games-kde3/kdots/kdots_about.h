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

#ifndef KDOTS_ABOUT_H
#define KDOTS_ABOUT_H

#include <qwidget.h>
#include <qevent.h>

class QPixmap;

class KDotsAbout : public QWidget
{
	Q_OBJECT
	public:
		KDotsAbout(QWidget *parent = NULL, const char *name = NULL);
		~KDotsAbout();
	public slots:
		void slotAccepted();
	protected:
		void paintEvent(QPaintEvent *e);
		void mouseMoveEvent(QMouseEvent *e);
		void mousePressEvent(QMouseEvent *e);
	private:
		QString measure(QString s);
		int m_repaint;
		int m_highlight;
		QFont m_font;
		QPixmap *m_bg;
};

#endif

