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
// KGGZMotd: Load the server specific Message of the Day. Parses and colorizes it. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZMotd.h"

// KGGZ includes
#include "KGGZCommon.h"
#include "KGGZCaption.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qpushbutton.h>
#include <qlayout.h>

// System includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constructor
KGGZMotd::KGGZMotd(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QPushButton *button;
	KGGZCaption *caption;
	QVBoxLayout *vbox;

	m_edit = new QTextView(this);
	m_edit->setGeometry(5, 5, 310, 360);
	m_edit->setFont(QFont("Courier", 10/*, QFont::Bold*/));

	button = new QPushButton("OK", this);
	//button->setGeometry(100, 370, 120, 20);

	caption = new KGGZCaption("MOTD", "GGZ Gaming Zone message of the day", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(caption);
	vbox->add(m_edit);
	vbox->add(button);

	connect(button, SIGNAL(clicked()), SLOT(close()));

	setCaption(i18n("Message Of The Day (MOTD)"));
	setFixedSize(320, 400);

	show();

	startTimer(500);
}

// Destructor
KGGZMotd::~KGGZMotd()
{
}

// add more lines
void KGGZMotd::append(char *text)
{
	unsigned int i;
	int j, count;
	const char *html[] = {"000000", "20ff00", "0000ff", "ffa000", "ff00ff", "300fff", "a0d000", "00ff70", "ff20ff", "777777", "AAAAAA"};
	QString buffer;

	KGGZDEBUGF("KGGZMotd::append(%s)\n", text);
	count = 0;
	for(i = 0; i < strlen(text); i++)
	{
		if(text[i] != '%')
		{
			switch(text[i])
			{
				case ' ':
					buffer.append("&nbsp;");
					break;
				case '<':
					buffer.append("&lt;");
					break;
				case '>':
					buffer.append("&gt;");
					break;
				case '\n':
					KGGZDEBUG("motd: end of line\n");
					//if((!i) || (text[i - i] != '\n'))
					//{
						buffer.append("<br>\n");
						count = 0;
					//}
					//else KGGZDEBUG("rejected!\n");
					break;
				default:
					buffer.append(text[i]);
			}
			count++;
			if(count > 37)
			{
				count = 0;
				buffer.append("<br>");
			}
		}
		else
		{
			i+=2;
			j = (int)text[i] - 48;
			if((j >= 0) && (j <= 9))
			{
				buffer.append("<font color=#");
				buffer.append(html[j]);
				buffer.append(">");

			}
		}
	}
	//KGGZDEBUG("append text now!\n");
	// This Qt bug is solved in the current CVS snapshot.
	//edit->append(buffer);
	m_edit->setText(m_edit->text() + buffer);
}

void KGGZMotd::setSource(void *data)
{
	char **motd;

	KGGZDEBUGF("KGGZMotd::setSource()\n");

	motd = (char**)data;
	if(!motd)
	{
		KGGZDEBUG("No MOTD data received!\n");
		return;
	}
	for(int i = 0; motd[i] != NULL; i++)
		append(motd[i]);
}
