/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Vencedor - The KDE 4 client for the GGZ Gaming Zone                  //
//    http://www.ggzgamingzone.org/clients/vencedor/                       //
//                                                                         //
//    Copyright (C) 2008, 2009 Josef Spillner <josef@ggzgamingzone.org>    //
//    Published under GNU GPL conditions - view COPYING for details        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    This program is free software; you can redistribute it and/or modify //
//    it under the terms of the GNU General Public License as published by //
//    the Free Software Foundation; either version 2 of the License, or    //
//    (at your option) any later version.                                  //
//                                                                         //
//    This program is distributed in the hope that it will be useful,      //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of       //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
//    GNU General Public License for more details.                         //
//                                                                         //
//    You should have received a copy of the GNU General Public License    //
//    along with this program; if not, write to the Free Software          //
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA        //
//    02110-1301 USA.                                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef VENCEDOR_MOTD_H
#define VENCEDOR_MOTD_H

#include <qdialog.h>

class QTextBrowser;
class KHTMLPart;

class Motd : public QDialog
{
	Q_OBJECT
	public:
		Motd(QWidget *parent);
		~Motd();
		void setText(QString motd);
		void setWebpage(QString url);

	private:
		void append(QString text);

		QTextBrowser *m_textmotd;
		KHTMLPart *m_webmotd;
};

#endif
