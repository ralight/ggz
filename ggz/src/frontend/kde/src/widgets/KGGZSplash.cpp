/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
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
// KGGZSplash: Show a splash screen on startup of KGGZ. This is overload but cool. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZSplash.h"

// KGGZ includes
#include "KGGZCommon.h"

// Qt includes
#include <qpixmap.h>
#include <qpainter.h>

KGGZSplash::KGGZSplash(QWidget *parent, const char *name)
: QWidget(parent, name, Qt::WNoAutoErase)
{
	setBackgroundColor(QColor(100, 0, 0));
}

KGGZSplash::~KGGZSplash()
{
}

void KGGZSplash::paintEvent(QPaintEvent *e)
{
	QPixmap splash(KGGZ_DIRECTORY "/images/splash/kggzsplash.png");
	QPixmap buffer(size());
	QPainter p;
	int x, y;

	x = (width() - splash.width()) / 2;
	y = (height() - splash.height()) / 2;

	p.begin(&buffer, this);
	p.fillRect(0, 0, width(), height(), QColor(100, 0, 0));
	p.drawPixmap(x, y, splash);
	p.end();

	bitBlt(this, 0, 0, &buffer);
}

