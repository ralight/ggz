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
// KGGZCaption: Provides a unique way of adding some more information to a window. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZCaption.h"

// Qt includes
#include <qpalette.h>
#include <qlabel.h>
#include <qlayout.h>

KGGZCaption::KGGZCaption(QString caption, QString explanation, QWidget *parent, const char *name)
: QFrame(parent, name)
{
	QLabel *label, *label2;
	QPalette pal;
	QVBoxLayout *vbox;

	label = new QLabel(caption, this);
	label2 = new QLabel(explanation, this);

	QFont font("helvetica", 11);
	font.setBold(TRUE);
	label->setFont(font);

	setEraseColor(QColor(100, 0, 0));
	label->setEraseColor(QColor(100, 0, 0));
	label2->setEraseColor(QColor(100, 0, 0));

	pal = palette();
	pal.setColor(QColorGroup::Foreground, QColor(255, 255, 255));
	label->setPalette(pal);
	label2->setPalette(pal);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label);
	vbox->add(label2);

	setFrameStyle(Panel | Sunken);
	setFixedHeight(60);
	show();
}

KGGZCaption::~KGGZCaption()
{
}

