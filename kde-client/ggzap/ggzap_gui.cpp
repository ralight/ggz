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
#include "ggzap_gui.h"

// KDE includes
#include <klocale.h>
#include <kapplication.h>

// Qt includes
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>

// Configuration files
#include "config.h"

GGZapGui::GGZapGui(QWidget *parent, const char *name)
: GGZapSkin(parent, name, WStyle_Customize | WRepaintNoErase)
{
	setPaletteBackgroundColor(QColor(30, 20, 140));
	setFixedSize(300, 200);

	setMouseTracking(1);
	m_x = -1;
	m_y = -1;
	m_progress = 0;
	m_hide = 0;
	m_help = 0;

	startTimer(200);
}

GGZapGui::~GGZapGui()
{
}

void GGZapGui::mouseMoveEvent(QMouseEvent *e)
{
	if((m_x != -1) || (m_y != -1))
		move(e->globalX() - m_x, e->globalY() - m_y);
}

void GGZapGui::mousePressEvent(QMouseEvent *e)
{
	m_x = e->x();
	m_y = e->y();

}

void GGZapGui::mouseReleaseEvent(QMouseEvent *e)
{
	m_x = -1;
	m_y = -1;
}

void GGZapGui::paintEvent(QPaintEvent *e)
{
	QPainter p;

	p.begin(this);
	p.setFont(QFont("arial", 24, QFont::Bold));
	p.setPen(QColor(255, 255, 255));
	p.drawText(20, 50, "GGZap");
	p.setFont(QFont("arial", 10, QFont::Bold));

	p.setPen(QColor(255, 255, 0));
	if(m_progress < 1) p.setPen(QColor(255, 255, 255));
	p.drawText(20, 100, i18n("Connecting to server"));
	if(m_progress < 2) p.setPen(QColor(255, 255, 255));
	p.drawText(20, 120, i18n("Logging in"));
	if(m_progress < 3) p.setPen(QColor(255, 255, 255));
	p.drawText(20, 140, i18n("Joining room"));
	if(m_progress < 4) p.setPen(QColor(255, 255, 255));
	p.drawText(20, 160, i18n("Waiting..."));
	if(m_progress < 5) p.setPen(QColor(255, 255, 255));
	p.drawText(20, 180, i18n("Launch game"));

	QFontMetrics m(this->font());
	int w = m.width(m_game);
	int h = m.height();
	QPixmap pix(m_icon);

	p.setPen(QColor(255, 255, 255));
	p.drawText(width() - w - 20, h + 20, m_game);
	p.drawPixmap(width() - pix.width() - 20, height() - 20 - pix.height(), pix);
	p.end();
}

void GGZapGui::setProgress(int progress)
{
	m_progress = progress;
	repaint();
}

void GGZapGui::setGame(QString game)
{
	m_game = game;
	m_icon = QString("%1/images/icons/games/%2.png").arg(KGGZ_DIRECTORY).arg(game);
}

void GGZapGui::timerEvent(QTimerEvent *e)
{
	static int a = 0;
	a++;
	setPaletteBackgroundColor(QColor(30, 20 + a, 140));

	repaint();
}

