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
#include <qbitmap.h>
#include <qpixmap.h>

KGGZSplash::KGGZSplash(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WResizeNoErase)
{
	m_list.append(KGGZ_DIRECTORY "/images/splash/ttt.png");
	m_list.append(KGGZ_DIRECTORY "/images/splash/dots.png");

	m_kggz = new QWidget(this);
	QPixmap p_kggz(KGGZ_DIRECTORY "/images/splash/title-kggz.png");
	QBitmap b_kggz(KGGZ_DIRECTORY "/images/splash/title-kggz-mask.png");
	m_kggz->setBackgroundPixmap(p_kggz);
	m_kggz->setMask(b_kggz);
	m_kggz->setFixedSize(p_kggz.width(), p_kggz.height());
	m_kggz->show();

	m_ggz = new QWidget(this);
	QPixmap p_ggz(KGGZ_DIRECTORY "/images/splash/title-ggz.png");
	QBitmap b_ggz(KGGZ_DIRECTORY "/images/splash/title-ggz-mask.png");
	m_ggz->setBackgroundPixmap(p_ggz);
	m_ggz->setMask(b_ggz);
	m_ggz->setFixedSize(p_ggz.width(), p_ggz.height());
	m_ggz->show();

	m_version = new QWidget(this);
	QPixmap p_version(KGGZ_DIRECTORY "/images/splash/title-version.png");
	QBitmap b_version(KGGZ_DIRECTORY "/images/splash/title-version-mask.png");
	m_version->setBackgroundPixmap(p_version);
	m_version->setMask(b_version);
	m_version->setFixedSize(p_version.width(), p_version.height());
	m_version->show();

	setBackgroundColor(QColor(90, 0, 0));

	startTimer(2000);

	show();
}

KGGZSplash::~KGGZSplash()
{
}

void KGGZSplash::paintTV()
{
	QPixmap pix(*m_list.at(m_current));
	bitBlt(this, 20, 20, &pix, 0, 0, pix.width(), pix.height(), CopyROP);
}

void KGGZSplash::paintTitle()
{
	m_kggz->move(50, height() - 250);
	m_version->move(50, height() - 150);
	m_ggz->move(50, height() - 100);
}

void KGGZSplash::timerEvent(QTimerEvent *e)
{
	m_current++;
	if(m_current >= m_list.count()) m_current = 0;

	paintTV();
}

void KGGZSplash::paintEvent(QPaintEvent *e)
{
	paintTitle();
	paintTV();
}

