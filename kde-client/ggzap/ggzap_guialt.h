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

#ifndef GGZAP_GUIALT_H
#define GGZAP_GUIALT_H

// GGZap includes
#include "ggzap_skin.h"

// Qt includes
#include <qwidget.h>
#include <qevent.h>

// Next generation GUI for GGZap
class GGZapGuiAlt : public GGZapSkin
{
	public:
		GGZapGuiAlt(QWidget *parent = NULL, const char *name = NULL);
		~GGZapGuiAlt();
		void setProgress(int progress);
		void setGame(QString game);

	protected:
		void mouseMoveEvent(QMouseEvent *e);
		void mousePressEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);
		void paintEvent(QPaintEvent *e);

	private:
		int m_x, m_y;
		int m_progress;
		QString m_game, m_icon;
		int m_hide, m_help;
};

#endif

