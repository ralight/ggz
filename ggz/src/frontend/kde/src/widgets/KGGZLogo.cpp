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

// System includes
#include <sys/stat.h>

KGGZLogo::KGGZLogo(QWidget *parent, const char *name)
: QFrame(parent, name)
{
}

KGGZLogo::~KGGZLogo()
{
}

void KGGZLogo::setLogo(char *logo, char *name)
{
	//char buffer[1024];
	QString *buffer;
	struct stat st;
	const char *uselogo;

	buffer = NULL;
	KGGZDEBUG("Found module icon: %s\n", logo);
	if(!logo)
	{
		//strcpy(buffer, KGGZ_DIRECTORY "/images/icons/games/");
		//strcat(buffer, name);
		//strcat(buffer, ".png");
		buffer = new QString(KGGZ_DIRECTORY "/images/icons/games/");
		buffer->append(name);
		buffer->append(".png");
		logo = (char*)buffer->latin1();
		KGGZDEBUG("Retrieve Icon from: %s\n", logo);
		if((stat(logo, &st) < 0) || (!S_ISREG(st.st_mode))) logo = NULL;
	}
	if(!logo) uselogo = KGGZ_DIRECTORY "/images/icons/module.png";
	else uselogo = logo;
	setBackgroundPixmap(QPixmap(uselogo));
	if(buffer) delete buffer;
}

void KGGZLogo::mousePressEvent(QMouseEvent *e)
{
	KGGZDEBUG("emit signalInfo()");
	emit signalInfo();
}

void KGGZLogo::shutdown()
{
	KGGZDEBUGF("KGGZLogo::shutdown()\n");
	erase();
	setBackgroundPixmap(NULL);
}
