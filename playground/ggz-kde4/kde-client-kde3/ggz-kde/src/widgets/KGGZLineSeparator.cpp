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
// KGGZLineSeparator: A style widget to separate user interface elements visually. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZLineSeparator.h"

KGGZLineSeparator::KGGZLineSeparator(QWidget *parent, const char *name)
: QFrame(parent, name)
{
	setFrameStyle(Panel | Sunken);
	setEraseColor(QColor(150, 0, 0));
	setDirection(horizontal);
	show();
}

KGGZLineSeparator::~KGGZLineSeparator()
{
}

void KGGZLineSeparator::setDirection(Directions direction)
{
	if(direction == horizontal)
	{
		setFixedHeight(3);
		setMinimumWidth(parentWidget()->minimumWidth());
		setMaximumWidth(parentWidget()->maximumWidth());
	}
	else
	{
		setFixedWidth(3);
		setMinimumHeight(parentWidget()->minimumHeight());
		setMaximumHeight(parentWidget()->maximumHeight());
	}
}

