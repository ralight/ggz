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
#include <khtml_part.h>
#include <khtmlview.h>

// Qt includes
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtextview.h>

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
	QFont f;

	m_textmotd = new QTextView(this);
	m_textmotd->setGeometry(5, 5, 310, 360);

	f = QFont("Courier", 10);
	f.setFixedPitch(true);
	f.setStyleHint(QFont::TypeWriter);

	m_textmotd->setFont(f);

	m_webmotd = new KHTMLPart(this);
	m_webmotd->hide();

	button = new QPushButton(i18n("OK"), this);

	caption = new KGGZCaption(i18n("MOTD"),
		i18n("GGZ Gaming Zone message of the day"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(caption);
	vbox->add(m_textmotd);
	vbox->add(m_webmotd->view());
	vbox->add(button);

	connect(button, SIGNAL(clicked()), SLOT(close()));

	setCaption(i18n("Message Of The Day (MOTD)"));
	resize(400, 400);
}

// Destructor
KGGZMotd::~KGGZMotd()
{
}

// add more lines
void KGGZMotd::append(QString text)
{
	unsigned int i;
	int j, count;
	const char *html[] =
	{
		"000000", "20ff00", "0000ff", "ffa000",
		"ff00ff", "300fff", "a0d000", "00ff70",
		"ff20ff", "777777", "AAAAAA"
	};
	QString buffer;

	count = 0;
	for(i = 0; i < text.length(); i++)
	{
		if(text[i] != '%')
		{
			switch(text[i].latin1())
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
					buffer.append("<br>\n");
					count = 0;
					break;
				default:
					buffer.append(text[i]);
			}
			count++;
		}
		else
		{
			i += 2;
			j = text[i].latin1() - 48;
			if((j >= 0) && (j <= 9))
			{
				buffer.append("<font color=#");
				buffer.append(html[j]);
				buffer.append(">");

			}
		}
	}

	m_textmotd->append(buffer);
}

void KGGZMotd::setWebpage(QString url)
{
	KGGZDEBUG("MOTD Webpage: %s!\n", url.latin1());
	if(url.isNull()) return;

	m_textmotd->hide();
	m_webmotd->show();

	m_webmotd->openURL(url);
}

void KGGZMotd::setSource(QString motd)
{
	m_textmotd->setText(motd);
}

