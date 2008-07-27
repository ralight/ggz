/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#ifndef KDOTS_WIN_H
#define KDOTS_WIN_H

#include <kmainwindow.h>
#include <kmenubar.h>

#include <kggzmod/module.h>

class KDots;
class KDotsAbout;
class KDotsHelp;
class KDotsReplay;
class QAction;

class KGGZRankingsDialog;
class KGGZSeatsDialog;

class KDotsWin : public KMainWindow
{
	Q_OBJECT
	public:
		KDotsWin(bool ggzmode);
		~KDotsWin();

	public Q_SLOTS:
		void slotMenu(QAction *action);
		void slotStatus(QString);
		void slotColor(const QColor& color);

	private:
		KDots *m_dots;
		KDotsAbout *kdots_about;
		KDotsHelp *kdots_help;
		KDotsReplay *kdots_replay;
		QWidget *m_color;

		QAction *action_help;
		QAction *action_about;
		QAction *action_quit;
		QAction *action_sync;
		QAction *action_replay;
		QAction *action_seats;
		QAction *action_rankings;

		KGGZRankingsDialog *m_rankings;
		KGGZSeatsDialog *m_seats;
};

#endif
