// GGZap - GGZ quick start application for the KDE panel
// Copyright (C) 2001 - 2003 Josef Spillner, josef@ggzgamingzone.org
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

#ifndef GGZAP_SKIN_H
#define GGZAP_SKIN_H

// Qt includes
#include <qwidget.h>

// Common GUI class
class GGZapSkin : public QWidget
{
	public:
		GGZapSkin(QWidget *parent = NULL, const char *name = NULL, int flags = 0);
		virtual ~GGZapSkin();
		virtual void setProgress(int progress) = 0;
		virtual void setGame(QString game) = 0;
};

#endif

