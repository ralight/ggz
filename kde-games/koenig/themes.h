// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
// Copyright (C) 2001 - 2004 Josef Spillner <josef@ggzgamingzone.org>
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

#ifndef CHESS_THEMES_H
#define CHESS_THEMES_H

#include <qwidget.h>
#include <qmap.h>

class QComboBox;

class Themes : public QWidget
{
	Q_OBJECT
	public:
		Themes(QWidget *parent = NULL, const char *name = NULL);
		~Themes();
	public slots:
		void slotTheme();
	signals:
		void signalTheme(QString theme);
	private:
		void scanDir(QString directory);
		void scan();
		QComboBox *m_themes;
		QMap<QString, QString> m_thememap;
};

#endif

