/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4                //
//    Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net      //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZSplashScreen: The widget which does actually display the splash front page. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZSplashScreen.h"

// KGGZ includes
#include "KGGZCommon.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qpixmap.h>
#include <qlayout.h>
#include <qpainter.h>

KGGZSplashScreen::KGGZSplashScreen(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	setBackgroundColor(QColor(0.0, 0.0, 255.0));
	setBackgroundPixmap(QPixmap(KGGZ_DIRECTORY "/images/startup.png"));
	setMaximumSize(500, 400);
}

KGGZSplashScreen::~KGGZSplashScreen()
{
}

void KGGZSplashScreen::paintEvent(QPaintEvent *e)
{
	QPainter p;

	p.begin(this);
	p.drawText(60, 273, i18n("Playing open source online games world-wide."));
	p.drawText(60, 312, i18n("Selecting from more than 15 games."));
	p.end();
}

