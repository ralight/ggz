// Keepalive - Experimental GGZ game
// Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef KEEPALIVE_INTRO_H
#define KEEPALIVE_INTRO_H

// Qt includes
#include <qwidget.h>
#include <qimage.h>

// Forward declarations
class QPixmap;
class QLineEdit;

// Graphics-intensive login and avatar managing class
class Intro : public QWidget
{
	Q_OBJECT
	public:
		Intro(QWidget *parent = NULL, const char *name = NULL);
		~Intro();
		void enableGGZ();

	signals:
		void signalLogin(QString username, QString password, QString hostname);

	protected:
		void timerEvent(QTimerEvent *e);
		void paintEvent(QPaintEvent *e);
		void mousePressEvent(QMouseEvent *e);

	private:
		void draw(QImage im, QImage bm, int posx, int posy, int blend);
		void drawall();
		void drawalllogin();
		void drawalllogin2();
		void drawallscores();
		void drawallavatar();
		void drawallavatar2();
		void drawallblack();

		int blendval_title, blendval_login;
		QImage imbuf, im, im_title, im_titlebm;
		QImage im_arrow, im_arrowbm, im_login, im_loginbm;
		QImage im_avatar, im_avatarbm, im_scores, im_scoresbm;
		QImage im_black, im_blackbm;

		QLineEdit *edit_username, *edit_password, *edit_avatar, *edit_hostname;
		int arrownum;
		int forcedraw;

		enum Transitions
		{
			none,
			nonetomain,
			maintonone,
			nonetoscores,
			scorestonone,
			nonetoavatar,
			avatartonone,
			nonetologin,
			logintonone,
			nonetoblack
		};

		enum Screens
		{
			screenmain,
			screenlogin,
			screenlogin2,
			screenavatar,
			screenavatar2,
			screenscores,
			screenblack
		};

		int transition;
		int transitiontarget;
		int screen;
		int screentarget;

		int avatar;
		int avatarnum;
		int selectedavatar, selectedworld;
		int delayselect;

		QString username, password, hostname;

		int m_ggz;
};

#endif

