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

// Header file
#include "intro.h"

// Keepalive includes
#include "unitfactory.h"
#include "config.h"

// Qt includes
#include <qpixmap.h>
#include <qbitmap.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qcanvas.h>

// KDE includes
#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>

// Useful definitions
#define min(x, y) (x < y ? x : y)
#define KEEPALIVE_DIR GGZDATADIR "/keepalive"

Intro::Intro(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QBitmap titlebm, loginbm, arrowbm, avatarbm, scoresbm;

	QPixmap background(KEEPALIVE_DIR "/intro.png");
	QPixmap title(KEEPALIVE_DIR "/introfont.png");
	QPixmap login(KEEPALIVE_DIR "/intrologin.png");
	QPixmap arrow(KEEPALIVE_DIR "/introarrow.png");
	QPixmap avatarpix(KEEPALIVE_DIR "/introavatar.png");
	QPixmap scores(KEEPALIVE_DIR "/introscores.png");

	titlebm = title.createHeuristicMask();
	loginbm = login.createHeuristicMask();
	arrowbm = arrow.createHeuristicMask();
	avatarbm = avatarpix.createHeuristicMask();
	scoresbm = scores.createHeuristicMask();

	im = background.convertToImage();

	im_title = title.convertToImage();
	im_titlebm = titlebm.convertToImage();
	im_login = login.convertToImage();
	im_loginbm = loginbm.convertToImage();
	im_arrow = arrow.convertToImage();
	im_arrowbm = arrowbm.convertToImage();
	im_avatar = avatarpix.convertToImage();
	im_avatarbm = avatarbm.convertToImage();
	im_scores = scores.convertToImage();
	im_scoresbm = scoresbm.convertToImage();

	setFixedSize(background.width(), background.height());

	setErasePixmap(background);

	m_ggz = 0;

	blendval_title = 0;
	blendval_login = -1;

	avatar = 0;
	selectedavatar = 0;
	selectedworld = 0;
	delayselect = -1;

	arrownum = 0;
	forcedraw = 0;
	transition = nonetomain;
	transitiontarget = none;
	screentarget = screenmain;
	screen = screenmain;

	edit_username = new QLineEdit(this);
	edit_username->setFixedSize(120, 18);
	edit_username->move(300, 300);
	edit_username->hide();

	edit_password = new QLineEdit(this);
	edit_password->setFixedSize(120, 18);
	edit_password->move(300, 340);
	edit_password->hide();

	edit_hostname = new QLineEdit(this);
	edit_hostname->setFixedSize(120, 18);
	edit_hostname->move(300, 380);
	edit_hostname->hide();

	edit_avatar = new QLineEdit(this);
	edit_avatar->setFixedSize(120, 18);
	edit_avatar->move(50, 320);
	edit_avatar->hide();

	startTimer(80);
}

Intro::~Intro()
{
}

void Intro::timerEvent(QTimerEvent *e)
{
	Q_UNUSED(e);

	if(screen == screenmain) drawall();
	if(screen == screenlogin) drawalllogin();
	if(screen == screenlogin2) drawalllogin2();
	if(screen == screenavatar) drawallavatar();
	if(screen == screenavatar2) drawallavatar2();
	if(screen == screenscores) drawallscores();
	if(screen == screenblack) drawallblack();

	if(delayselect >= 0) delayselect--;
	if(!delayselect)
	{
		arrownum = 1;
		transition = avatartonone;
		transitiontarget = nonetomain;
		screentarget = screenmain;
		forcedraw = 1;
	}
}

void Intro::drawalllogin()
{
	int titleinc = 0;
	QPixmap pix;
	KConfig *conf;
	QStringList avatarnames, avatartypes;

	if(transition == none) return;

	imbuf = im.copy();

	if(transition == nonetologin) titleinc = 2;
	else if(transition == logintonone) titleinc = -4;

	if(((blendval_login >= 0) && (blendval_login < 97)) || (forcedraw))
	{
		draw(im_login, im_loginbm, 110, 50, blendval_login);

		if(arrownum == 1) draw(im_arrow, im_arrowbm, 20, 20, 50);
		else draw(im_arrow, im_arrowbm, 20, 20, blendval_login);

		if(((titleinc > 0) && (blendval_login < 97))
		|| ((titleinc < 0) && (blendval_login > 0))) blendval_login += titleinc * 2;

		if(blendval_login == 100)
		{
			QPainter p;
			UnitFactory *f;
			QCanvasPixmapArray *av1;

			conf = kapp->config();
			conf->setGroup("Avatars");
			avatarnames = conf->readListEntry("Names");
			avatartypes = conf->readListEntry("Types");

			f = new UnitFactory();
			av1 = f->load(*avatartypes.at(selectedavatar - 1));
			delete f;

			pix.convertFromImage(imbuf);
			p.begin(&pix);
			p.setPen(QColor(255, 255, 255));
			p.setFont(QFont("", 16, QFont::Bold));
			p.drawText(300, 295, "Username");
			p.drawText(300, 335, "Password");
			if(!m_ggz)
			{
				p.drawText(300, 375, "Hostname");
			}
			p.setPen(QColor(255, 255, 0));
			p.drawText(300, 415, "Login");
			if((av1) && (av1->count()))
			{
				p.drawPixmap(320, 150, *(av1->image(5)));
				p.setPen(QColor(255, 255, 255));
				p.drawText(300, 230, *avatarnames.at(selectedavatar - 1));
			}
			p.end();
			imbuf = pix.convertToImage();

			if(av1) delete av1;

			edit_username->show();
			edit_password->show();
			if(!m_ggz)
			{
				edit_hostname->show();
			}

			transition = none;
			forcedraw = 1;
		}
		else
		{
			edit_username->hide();
			edit_password->hide();
			edit_hostname->hide();
		}

		if(blendval_login <= 0)
		{
			blendval_login = 0;
			screen = screentarget;
			transition = transitiontarget;
			arrownum = 0;
		}
	}

	if((blendval_login < 100) || (forcedraw)) bitBlt(this, 0, 0, &imbuf, 0, 0, width(), height());

	forcedraw = 0;
}

void Intro::drawalllogin2()
{
	QPixmap pix;

	if(forcedraw) imbuf = im.copy();

	if(((blendval_login >= 0) && (blendval_login < 97)) || (forcedraw))
	{
		draw(im_login, im_loginbm, 110, 50, blendval_login);

		if(arrownum == 1) draw(im_arrow, im_arrowbm, 20, 20, 50);
		else draw(im_arrow, im_arrowbm, 20, 20, blendval_login);

		if(transition != none) blendval_login -= 8;

		if(blendval_login == 100)
		{
			QPainter p;

			pix.convertFromImage(imbuf);
			p.begin(&pix);
			p.setPen(QColor(255, 255, 255));
			p.setFont(QFont("", 16, QFont::Bold));
			p.drawText(50, 200, "Select a world");
			p.drawPixmap(50, 300, QPixmap(KEEPALIVE_DIR "/world1.png"));
			p.drawPixmap(250, 300, QPixmap(KEEPALIVE_DIR "/world2.png"));
			p.setPen(QColor(255, 255, 0));
			p.drawText(50, 400, "noosphere");
			p.drawText(250, 400, "résistance");
			p.end();
			imbuf = pix.convertToImage();

			edit_username->hide();
			edit_password->hide();
			edit_hostname->hide();
		}

		if(blendval_login <= 0)
		{
			blendval_login = 0;
			screen = screentarget;
			transition = transitiontarget;
			arrownum = 0;
		}
	}

	if((blendval_login < 100) || (forcedraw)) bitBlt(this, 0, 0, &imbuf, 0, 0, width(), height());

	forcedraw = 0;
}

void Intro::drawallblack()
{
	QPixmap pix;
	QImage imtmp;

	if(transitiontarget == none) return;

	if(forcedraw) imbuf = im.copy();

	if(((blendval_login >= 0) && (blendval_login < 97)) || (forcedraw))
	{
		if(!blendval_login)
		{
			im_black = QImage(width(), height(), 8, 2);
			im_black.fill(0);
			im_blackbm = QBitmap(width(), height());
			im_blackbm.fill(0xFFFFFFFF);
		}

		draw(im_black, im_blackbm, 0, 0, blendval_login);

		blendval_login += 8;

		if(blendval_login >= 100)
		{
//kdDebug() << "Game starts!" << endl;
			blendval_login = 0;
			transition = none;
			transitiontarget = none;
			arrownum = 0;

			emit signalLogin(username, password, hostname);
		}
	}

	if((blendval_login < 100) || (forcedraw)) bitBlt(this, 0, 0, &imbuf, 0, 0, width(), height());

	forcedraw = 0;
}

void Intro::drawallavatar()
{
	int titleinc = 0;
	QPixmap pix;
	QStringList avatarnames, avatartypes, avatarscores;
	KConfig *conf;
	QImage imtmp, imtmpbm;

	if((transition == none) && (!forcedraw)) return;

	imbuf = im.copy();

	if(transition == nonetoavatar) titleinc = 2;
	else if(transition == avatartonone) titleinc = -4;
	else titleinc = 0;

	if(((blendval_login >= 0) && (blendval_login < 97)) || (forcedraw))
	{
		draw(im_avatar, im_avatarbm, 110, 50, blendval_login);

		if(arrownum == 1) draw(im_arrow, im_arrowbm, 20, 20, 50);
		else draw(im_arrow, im_arrowbm, 20, 20, blendval_login);

		if(((titleinc > 0) && (blendval_login < 97))
		|| ((titleinc < 0) && (blendval_login > 0))) blendval_login += titleinc * 2;

		if(blendval_login == 100)
		{
			QPainter p;
			QCanvasPixmapArray *av1, *av2, *av3;
			UnitFactory *f;

			conf = kapp->config();
			conf->setGroup("Avatars");
			avatarnames = conf->readListEntry("Names");
			avatartypes = conf->readListEntry("Types");
			avatarscores = conf->readListEntry("Scores");

			f = new UnitFactory();
			if(avatarnames.count() > 0) av1 = f->load(*avatartypes.at(0));
			else av1 = NULL;
			if(avatarnames.count() > 1) av2 = f->load(*avatartypes.at(1));
			else av2 = NULL;
			if(avatarnames.count() > 2) av3 = f->load(*avatartypes.at(2));
			else av3 = NULL;
			delete f;
			avatarnum = avatarnames.count();

			pix.convertFromImage(imbuf);
			p.begin(&pix);
			p.setPen(QColor(255, 255, 0));
			p.setFont(QFont("", 16, QFont::Bold));
			if(avatarnum < 3) p.drawText(50, 200, "Create a new avatar");
			p.setPen(QColor(255, 255, 255));
			p.drawText(50, 240, "Current avatars");
			if((av1) && (av1->count()))
			{
				p.drawText(150, 270, *avatarnames.at(0));
				p.drawText(150, 290, *avatarscores.at(0) + " points");
			}
			if((av2) && (av2->count()))
			{
				p.drawText(150, 340, *avatarnames.at(1));
				p.drawText(150, 360, *avatarscores.at(1) + " points");
			}
			if((av3) && (av3->count()))
			{
				p.drawText(150, 410, *avatarnames.at(2));
				p.drawText(150, 430, *avatarscores.at(2) + " points");
			}
			p.end();
			imbuf = pix.convertToImage();

			if((av1) && (av1->count()))
			{
				imtmp = av1->image(4)->convertToImage();
				imtmpbm = av1->image(4)->createHeuristicMask().convertToImage();
				if(avatar == 1) draw(imtmp, imtmpbm, 50, 250, 100);
				else draw(imtmp, imtmpbm, 50, 250, 40);
			}
			if((av2) && (av2->count()))
			{
				imtmp = av2->image(4)->convertToImage();
				imtmpbm = av2->image(4)->createHeuristicMask().convertToImage();
				if(avatar == 2) draw(imtmp, imtmpbm, 50, 320, 100);
				else draw(imtmp, imtmpbm, 50, 320, 40);
			}
			if((av3) && (av3->count()))
			{
				imtmp = av3->image(4)->convertToImage();
				imtmpbm = av3->image(4)->createHeuristicMask().convertToImage();
				if(avatar == 3) draw(imtmp, imtmpbm, 50, 390, 100);
				else draw(imtmp, imtmpbm, 50, 390, 40);
			}

			transition = none;
			forcedraw = 1;
		}

		if(blendval_login <= 0)
		{
			blendval_login = 0;
			screen = screentarget;
			transition = transitiontarget;
			arrownum = 0;
		}
	}

	if((blendval_login < 100) || (forcedraw)) bitBlt(this, 0, 0, &imbuf, 0, 0, width(), height());

	forcedraw = 0;
}

void Intro::drawallavatar2()
{
	QPixmap pix;
	QImage imtmp, imtmpbm;

	if(forcedraw) imbuf = im.copy();

	if(((blendval_login >= 0) && (blendval_login < 97)) || (forcedraw))
	{
		draw(im_avatar, im_avatarbm, 110, 50, blendval_login);

		if(arrownum == 1) draw(im_arrow, im_arrowbm, 20, 20, 50);
		else draw(im_arrow, im_arrowbm, 20, 20, blendval_login);

		if(transition != none) blendval_login -= 8;

		if(blendval_login == 100)
		{
			QPainter p;
			QCanvasPixmapArray *av1, *av2, *av3;
			UnitFactory *f;

			f = new UnitFactory();
			av1 = f->load("archer");
			av2 = f->load("knight");
			av3 = f->load("mage");
			delete f;

			pix.convertFromImage(imbuf);
			p.begin(&pix);
			p.setPen(QColor(255, 255, 255));
			p.setFont(QFont("", 16, QFont::Bold));
			p.drawText(50, 200, "Select an avatar");
			p.drawText(50, 310, "Avatar name");
			p.setPen(QColor(255, 255, 0));
			p.drawText(50, 370, "Create");
			p.end();
			imbuf = pix.convertToImage();

			if((av1) && (av1->count()))
			{
				imtmp = av1->image(3)->convertToImage();
				imtmpbm = av1->image(3)->createHeuristicMask().convertToImage();
				if(avatar == 1) draw(imtmp, imtmpbm, 50, 220, 100);
				else draw(imtmp, imtmpbm, 50, 220, 40);
			}
			if((av2) && (av2->count()))
			{
				imtmp = av2->image(3)->convertToImage();
				imtmpbm = av2->image(3)->createHeuristicMask().convertToImage();
				if(avatar == 2) draw(imtmp, imtmpbm, 150, 220, 100);
				else draw(imtmp, imtmpbm, 150, 220, 40);
			}
			if((av3) && (av3->count()))
			{
				imtmp = av3->image(3)->convertToImage();
				imtmpbm = av3->image(3)->createHeuristicMask().convertToImage();
				if(avatar == 3) draw(imtmp, imtmpbm, 250, 220, 100);
				else draw(imtmp, imtmpbm, 250, 220, 40);
			}

			if(av1) delete av1;
			if(av2) delete av2;
			if(av3) delete av3;

			edit_avatar->show();
		}

		if(blendval_login <= 0)
		{
			blendval_login = 0;
			screen = screentarget;
			transition = transitiontarget;
			arrownum = 0;
		}
	}

	if((blendval_login < 100) || (forcedraw)) bitBlt(this, 0, 0, &imbuf, 0, 0, width(), height());

	forcedraw = 0;
}

void Intro::drawallscores()
{
	int titleinc = 0;
	QPixmap pix;

	if(transition == none) return;

	imbuf = im.copy();

	if(transition == nonetoscores) titleinc = 2;
	else if(transition == scorestonone) titleinc = -4;

	if(((blendval_login >= 0) && (blendval_login < 97)) || (forcedraw))
	{
		draw(im_scores, im_scoresbm, 110, 50, blendval_login);

		if(arrownum == 1) draw(im_arrow, im_arrowbm, 20, 20, 50);
		else draw(im_arrow, im_arrowbm, 20, 20, blendval_login);

		if(((titleinc > 0) && (blendval_login < 97))
		|| ((titleinc < 0) && (blendval_login > 0))) blendval_login += titleinc * 2;

		if(blendval_login == 100)
		{
			QPainter p;

			pix.convertFromImage(imbuf);
			p.begin(&pix);
			p.setPen(QColor(255, 255, 255));
			p.setFont(QFont("", 16, QFont::Bold));
			p.drawText(150, 200, "9432 shiva (josef)");
			p.drawText(150, 220, "234 Grunzer (grubby)");
			p.drawText(150, 240, "62 A'hin (the lightning kid)");
			p.end();
			imbuf = pix.convertToImage();

			transition = none;
			forcedraw = 1;
		}

		if(blendval_login <= 0)
		{
			blendval_login = 0;
			screen = screentarget;
			transition = transitiontarget;
			arrownum = 0;
		}
	}

	if((blendval_login < 100) || (forcedraw)) bitBlt(this, 0, 0, &imbuf, 0, 0, width(), height());

	forcedraw = 0;
}

void Intro::drawall()
{
	int titleinc = 0;

	if(transition == none) return;

	imbuf = im.copy();

	if(transition == nonetomain) titleinc = 2;
	else if(transition == maintonone) titleinc = -4;

	if(((blendval_title >= 0) && (blendval_title < 99)) || (forcedraw))
	{
		draw(im_title, im_titlebm, 110, 50, blendval_title);
		if(((titleinc > 0) && (blendval_title < 99))
		|| ((titleinc < 0) && (blendval_title > 0))) blendval_title += titleinc;

		if((titleinc > 0) && (blendval_title == 50)) blendval_login = 0;
		if(blendval_title < 0) blendval_title = 0;
	}

	if(((blendval_login >= 0) && (blendval_login < 97)) || (forcedraw))
	{
		if(selectedavatar > 0) draw(im_login, im_loginbm, 110, 150, blendval_login);
		else draw(im_login, im_loginbm, 110, 150, min(blendval_login, 50));
		draw(im_avatar, im_avatarbm, 110, 200, blendval_login);
		draw(im_scores, im_scoresbm, 110, 250, blendval_login);

		if(arrownum == 1) draw(im_arrow, im_arrowbm, 60, 150, 50);
		else
		{
			if(selectedavatar > 0) draw(im_arrow, im_arrowbm, 60, 150, blendval_login);
			else draw(im_arrow, im_arrowbm, 60, 150, min(blendval_login, 50));
		}
		if(arrownum == 2) draw(im_arrow, im_arrowbm, 60, 200, 50);
		else draw(im_arrow, im_arrowbm, 60, 200, blendval_login);
		if(arrownum == 3) draw(im_arrow, im_arrowbm, 60, 250, 50);
		else draw(im_arrow, im_arrowbm, 60, 250, blendval_login);

		if(((titleinc > 0) && (blendval_login < 97))
		|| ((titleinc < 0) && (blendval_login > 0))) blendval_login += titleinc * 2;

		if(blendval_login == 100)
		{
			transition = none;
		}
		if(blendval_login <= 0)
		{
			blendval_login = 0;
			screen = screentarget;
			transition = transitiontarget;
			arrownum = 0;
		}
	}

	if((blendval_login < 100) || (forcedraw)) bitBlt(this, 0, 0, &imbuf, 0, 0, width(), height());

	forcedraw = 0;
}

void Intro::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);

	bitBlt(this, 0, 0, &imbuf, 0, 0, width(), height());
}

void Intro::draw(QImage image, QImage bm, int posx, int posy, int blend)
{
	QRgb rgba;
	int r, g, b, a;
	int r2, g2, b2;
	int r3, g3, b3;

	for(int j = 0; j < image.height(); j++)
		for(int i = 0; i < image.width(); i++)
		{
			rgba = image.pixel(i, j);
			r2 = qRed(rgba);
			g2 = qGreen(rgba);
			b2 = qBlue(rgba);

			rgba = im.pixel(posx + i, posy + j);
			r = qRed(rgba);
			g = qGreen(rgba);
			b = qBlue(rgba);

			r3 = (blend * r2 + (100 - blend) * r) / 100;
			g3 = (blend * g2 + (100 - blend) * g) / 100;
			b3 = (blend * b2 + (100 - blend) * b) / 100;

			a = 255;
			if(!(bm.pixel(i, j) & 1)) imbuf.setPixel(posx + i, posy + j, qRgba(r3, g3, b3, a));
		}
}

void Intro::mousePressEvent(QMouseEvent *e)
{
	int x, y;

	if(transition != none)
	{
		if((transition == nonetomain) || (transition == nonetologin)
		|| (transition == nonetoscores) || (transition == nonetoavatar))
		{
			blendval_login = 96;
			blendval_title = 98;
		}
		else if(transition == nonetoblack)
		{
			blendval_login = 92;
		}
		else
		{
			blendval_login = 8;
			blendval_title = 4;
		}
		forcedraw = 1;
		return;
	}

	x = e->x();
	y = e->y();

	arrownum = 0;

	if(screen == screenmain)
	{
		if((x >= 60) && (x <= 160) && (y >= 160) && (y < 210))
		{
			if(selectedavatar > 0)
			{
				arrownum = 1;
				transition = maintonone;
				transitiontarget = nonetologin;
				screentarget = screenlogin;
			}
		}
		if((x >= 60) && (x <= 160) && (y >= 210) && (y < 260))
		{
			arrownum = 2;
			transition = maintonone;
			transitiontarget = nonetoavatar;
			screentarget = screenavatar;
		}
		if((x >= 60) && (x <= 160) && (y >= 260) && (y < 310))
		{
			arrownum = 3;
			transition = maintonone;
			transitiontarget = nonetoscores;
			screentarget = screenscores;
		}
	}
	else if(screen == screenlogin)
	{
		if((x >= 20) && (x <= 120) && (y >= 20) && (y < 70))
		{
			arrownum = 1;
			transition = logintonone;
			transitiontarget = nonetomain;
			screentarget = screenmain;
		}
		if((x >= 300) && (x <= 340) && (y >= 400) && (y <= 420))
		{
			username = edit_username->text();
			password = edit_password->text();
			hostname = edit_hostname->text();
			if((!username.isEmpty()) && (!password.isEmpty()))
			{
				if((!hostname.isEmpty()) || (m_ggz)) screen = screenlogin2;
			}
		}
	}
	else if(screen == screenlogin2)
	{
		if((x >= 20) && (x <= 120) && (y >= 20) && (y < 70))
		{
			arrownum = 1;
			transition = logintonone;
			transitiontarget = nonetomain;
			screentarget = screenmain;
		}
		if((x >= 50) && (x <= 200) && (y >= 300) && (y <= 400))
		{
			transition = logintonone;
			transitiontarget = nonetoblack;
			screentarget = screenblack;
		}
		if((x >= 250) && (x <= 400) && (y >= 300) && (y <= 400))
		{
			transition = logintonone;
			transitiontarget = nonetoblack;
			screentarget = screenblack;
		}
	}
	else if(screen == screenavatar)
	{
		if((x >= 20) && (x <= 120) && (y >= 20) && (y <= 70))
		{
			arrownum = 1;
			transition = avatartonone;
			transitiontarget = nonetomain;
			screentarget = screenmain;
			delayselect = -1;
		}
		if((x >= 50) && (x <= 250) && (y >= 185) && (y <= 205))
		{
			if(avatarnum < 3)
			{
				edit_avatar->setText("");
				screen = screenavatar2;
				delayselect = -1;
			}
		}
		if((x >= 50) && (x <= 100) && (y >= 250) && (y <= 300))
		{
			if(avatarnum > 0)
			{
				avatar = 1;
				selectedavatar = 1;
				delayselect = 20;
			}
		}
		if((x >= 50) && (x <= 100) && (y >= 320) && (y <= 370))
		{
			if(avatarnum > 1)
			{
				avatar = 2;
				selectedavatar = 2;
				delayselect = 20;
			}
		}
		if((x >= 50) && (x <= 100) && (y >= 390) && (y <= 440))
		{
			if(avatarnum > 2)
			{
				avatar = 3;
				selectedavatar = 3;
				delayselect = 20;
			}
		}
	}
	else if(screen == screenavatar2)
	{
		if((x >= 20) && (x <= 120) && (y >= 20) && (y <= 70))
		{
			arrownum = 1;
			transition = avatartonone;
			transitiontarget = nonetomain;
			screentarget = screenmain;
		}
		if((x >= 50) && (x <= 250) && (y >= 355) && (y <= 375))
		{
			if((avatar) && (edit_avatar->text().length() > 3))
			{
				edit_avatar->hide();
				screen = screenavatar;

				KConfig *conf;
				QStringList avatarnames, avatartypes, avatarscores;

				conf = kapp->config();
				conf->setGroup("Avatars");
				avatarnames = conf->readListEntry("Names");
				avatartypes = conf->readListEntry("Types");
				avatarscores = conf->readListEntry("Scores");

				avatarnames += edit_avatar->text();
				if(avatar == 1) avatartypes += "archer";
				if(avatar == 2) avatartypes += "knight";
				if(avatar == 3) avatartypes += "mage";
				avatarscores += "0";

				conf->writeEntry("Names", avatarnames);
				conf->writeEntry("Types", avatartypes);
				conf->writeEntry("Scores", avatarscores);

				avatar = 0;
			}
		}
		if((x >= 50) && (x <= 100) && (y >= 220) && (y <= 270))
		{
			avatar = 1;
		}
		if((x >= 150) && (x <= 200) && (y >= 220) && (y <= 270))
		{
			avatar = 2;
		}
		if((x >= 250) && (x <= 300) && (y >= 220) && (y <= 270))
		{
			avatar = 3;
		}
	}
	else if(screen == screenscores)
	{
		if((x >= 20) && (x <= 120) && (y >= 20) && (y <= 70))
		{
			arrownum = 1;
			transition = scorestonone;
			transitiontarget = nonetomain;
			screentarget = screenmain;
		}
	}

	forcedraw = 1;
}

void Intro::enableGGZ()
{
	m_ggz = 1;
}

