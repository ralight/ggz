// GGZap - GGZ quick start application for the KDE panel
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
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
#include "ggzap_guialt.h"

// KDE includes
#include <klocale.h>
#include <kapplication.h>

// Qt includes
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>

// Configuration files
#include "config.h"

GGZapGuiAlt::GGZapGuiAlt(QWidget *parent, const char *name)
: GGZapSkin(parent, name, WStyle_Customize | WRepaintNoErase)
{
	QPixmap pix = QPixmap(KGGZ_DIRECTORY "/ggzap/redgear.png");
	setErasePixmap(pix);
	setFixedSize(pix.width(), pix.height());
	setMask(QBitmap(KGGZ_DIRECTORY "/ggzap/redgearmask.png"));
	setMouseTracking(1);
	m_x = -1;
	m_y = -1;
	m_progress = 0;
	m_hide = 0;
	m_help = 0;
}

GGZapGuiAlt::~GGZapGuiAlt()
{
}

void GGZapGuiAlt::mouseMoveEvent(QMouseEvent *e)
{
	int tmp;

	if((m_x != -1) || (m_y != -1))
		move(e->globalX() - m_x, e->globalY() - m_y);

	tmp = m_hide;
	if((e->x() > 240) && (e->x() < 270) && (e->y() > 340) && (e->y() < 357)) m_hide = 1;
	else m_hide = 0;
	if(tmp != m_hide) repaint(240, 340, 30, 17, false);

	tmp = m_help;
	if((e->x() > 240) && (e->x() < 270) && (e->y() > 360) && (e->y() < 377)) m_help = 1;
	else m_help = 0;
	if(tmp != m_help) repaint(240, 360, 30, 17, false);
}

void GGZapGuiAlt::mousePressEvent(QMouseEvent *e)
{
	m_x = e->x();
	m_y = e->y();

	if((m_x > 240) && (m_x < 270) && (m_y > 340) && (m_y < 357))
	{
		m_x = -1;
		m_y = -1;
		m_hide = 0;
		hide();
	}
	if((m_x > 240) && (m_x < 270) && (m_y > 360) && (m_y < 377))
	{
		kapp->invokeBrowser("http://ggz.sourceforge.net/clients/ggzap/");
		m_x = -1;
		m_y = -1;
	}

}

void GGZapGuiAlt::mouseReleaseEvent(QMouseEvent *e)
{
	m_x = -1;
	m_y = -1;
}

void GGZapGuiAlt::paintEvent(QPaintEvent *e)
{
	QPainter p;

	p.begin(this);
	p.setFont(QFont("arial", 24, QFont::Bold));
	p.setPen(QColor(255, 255, 255));
	p.drawText(200, 190, "GGZap");
	p.setFont(QFont("arial", 10, QFont::Bold));

	p.setPen(QColor(255, 255, 0));
	if(m_progress < 1) p.setPen(QColor(0, 0, 0));
	p.drawText(145, 220, i18n("Connecting to server"));
	if(m_progress < 2) p.setPen(QColor(0, 0, 0));
	p.drawText(135, 240, i18n("Logging in"));
	if(m_progress < 3) p.setPen(QColor(0, 0, 0));
	p.drawText(130, 260, i18n("Joining room"));
	if(m_progress < 4) p.setPen(QColor(0, 0, 0));
	p.drawText(135, 280, i18n("Waiting..."));
	if(m_progress < 5) p.setPen(QColor(0, 0, 0));
	p.drawText(145, 300, i18n("Launch game"));


	if(m_hide)
		p.setPen(QColor(255, 255, 0));
	else
		p.setPen(QColor(255, 255, 255));
	p.drawText(240, 350, i18n("Hide"));

	if(m_help)
		p.setPen(QColor(255, 255, 0));
	else
		p.setPen(QColor(255, 255, 255));
	p.drawText(240, 370, i18n("Help"));

	QFontMetrics m(this->font());
	int w = m.width(m_game);
	p.drawText(340 - w, 320, m_game);
	p.drawPixmap(290, 240, QPixmap(m_icon));
	p.end();
}

void GGZapGuiAlt::setProgress(int progress)
{
	m_progress = progress;
	repaint();
}

void GGZapGuiAlt::setGame(QString game)
{
	m_game = game;
	m_icon = QString("%1/images/icons/games/%2.png").arg(KGGZ_DIRECTORY).arg(game);
}

