/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#ifndef KDOTS_H
#define KDOTS_H

#include <qwidget.h>
#include <QEvent>

class KDotsProto;
class KDotsOptions;
class QDots;

class KDots : public QWidget
{
	Q_OBJECT
	public:
		KDots(bool ggzmode);
		~KDots();
		void gameinit();
		void gamesync();
		void input();

		KDotsProto *getProto();

	public Q_SLOTS:
		void slotOptions();
		void slotStart(int horizontal, int vertical);
		void slotTurn(int x, int y, int direction);
		void slotInput();
		void slotSync();
		/*void slotDispatch();*/

	Q_SIGNALS:
		void signalStatus(QString message);
		void signalColor(const QColor& color);

	private:
		int m_rows;
		int m_cols;
		int *m_field;
		KDotsProto *proto;
		KDotsOptions *kdots_options;
		QDots *dots;
};

#endif
