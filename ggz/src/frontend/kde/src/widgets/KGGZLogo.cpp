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
// KGGZLogo: Left upper widget in chat window. It's meant to show the active room. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZLogo.h"

// KGGZ includes
#include "KGGZCommon.h"

// Qt includes
#include <qpixmap.h>
#include <qlayout.h>

// System includes
#include <sys/stat.h>

KGGZLogo::KGGZLogo(QWidget *parent, const char *name)
: QFrame(parent, name)
{
	QVBoxLayout *vbox;

	setFrameStyle(QFrame::Panel | QFrame::Sunken);
	setBackgroundColor(QColor(150, 0, 0));
	setMinimumSize(64, 64);

	m_logo = new QFrame(this);
	//m_logo->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	m_logo->setBackgroundColor(QColor(150, 0, 0));

	vbox = new QVBoxLayout(this, 0);
	vbox->add(m_logo);

	m_logo->installEventFilter(this);
}

KGGZLogo::~KGGZLogo()
{
}

void KGGZLogo::setLogo(const char *logo, const char *name)
{
	QPixmap pix;
	QString buffer;
	struct stat st;
	const char *uselogo;

	KGGZDEBUG("Found module icon: %s\n", logo);
	uselogo = logo;

	if(!uselogo)
	{
		buffer = KGGZ_DIRECTORY "/images/icons/games/";
		buffer.append(name);
		buffer.append(".png");
		uselogo = buffer.latin1();
		KGGZDEBUG("Retrieve Icon from: %s\n", uselogo);
		if((stat(uselogo, &st) < 0) || (!S_ISREG(st.st_mode))) uselogo = NULL;
	}
	if(!uselogo) uselogo = KGGZ_DIRECTORY "/images/icons/module.png";

	pix = QPixmap(uselogo);
	m_logo->setBackgroundPixmap(pix);
	m_logo->setFixedSize(pix.width(), pix.height());
}

void KGGZLogo::mousePressEvent(QMouseEvent *e)
{
	emit signalInfo();
}

void KGGZLogo::shutdown()
{
	erase();
	m_logo->setBackgroundPixmap(NULL);
}

bool KGGZLogo::eventFilter(QObject *o, QEvent *e)
{
	if(e->type() == QEvent::MouseButtonPress)
	{
		emit signalInfo();
		return true;
	}
	return QWidget::eventFilter(o, e);
}

