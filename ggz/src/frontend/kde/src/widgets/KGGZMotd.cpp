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
// KGGZMotd: Load the server specific Message of the Day. Parses and colorizes it. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZMotd.h"

// KGGZ includes
#include "KGGZCommon.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qpushbutton.h>

// System includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constructor
KGGZMotd::KGGZMotd(QWidget *parent = NULL, char *name = NULL)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QPushButton *button;

	m_edit = new QTextView(this);
	m_edit->setGeometry(5, 5, 310, 360);
	m_edit->setFont(QFont("Courier", 8, QFont::Bold));

	button = new QPushButton("OK", this);
	button->setGeometry(100, 370, 120, 20);

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
	int i, j, count;
	char tmp[8];
	char *html[] = {"ff0000", "00ff00", "0000ff", "ffff00", "ff00ff", "0000ff", "ffff00", "00ffff", "ff00ff", "777777", "AAAAAA"};
	//char buf[1024];
	QString buffer;

	//KGGZDEBUGF("KGGZMotd::append(%s)\n", text);
	//KGGZDEBUGF("text length: %i\n", strlen(text));
	count = 0;
	//strcpy(buf, "");
	//buffer = new QString();
	for(i = 0; i < strlen(text); i++)
	{
		if(text[i] != '%')
		{
			switch(text[i])
			{
				case ' ':
					//sprintf(tmp, "&nbsp;");
					buffer.append("&nbsp;");
					break;
				case '<':
					//sprintf(tmp, "&lt;");
					buffer.append("&lt;");
					break;
				case '>':
					//sprintf(tmp, "&gt;");
					buffer.append("&gt;");
					break;
				default:
					//sprintf(tmp, "%c", text[i]);
					buffer.append(text[i]);
			}
			//strcat(buf, tmp);
			count++;
			if(count > 45)
			{
				count = 0;
				//strcat(buf, "<br>");
				buffer.append("<br>");
			}
		}
		else
		{
			i+=2;
			j = (int)text[i] - 48;
			if((j >= 0) && (j <= 9))
			{
				//strcat(buf, "<font color=#");
				//strcat(buf, html[j]);
				//strcat(buf, ">");
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
	m_edit->setText("GGZ Gaming Zone Message Of The Day:<br><br>");

	//KGGZDEBUG("--> cleared\n");
	motd = (char**)data;
	//KGGZDEBUG("--> casted\n");
	if(!motd)
	{
		KGGZDEBUG("No MOTD data received!\n");
		return;
	}
	//KGGZDEBUG("--> checked\n");
	for(int i = 0; motd[i] != NULL; i++)
	{
		//KGGZDEBUG("--> append: %s\n", motd[i]);
		append(motd[i]);
	}
}
