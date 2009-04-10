/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    Vencedor - The KDE 4 client for the GGZ Gaming Zone                  //
//    http://www.ggzgamingzone.org/clients/vencedor/                       //
//                                                                         //
//    Copyright (C) 2008, 2009 Josef Spillner <josef@ggzgamingzone.org>    //
//    Published under GNU GPL conditions - view COPYING for details        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//    This program is free software; you can redistribute it and/or modify //
//    it under the terms of the GNU General Public License as published by //
//    the Free Software Foundation; either version 2 of the License, or    //
//    (at your option) any later version.                                  //
//                                                                         //
//    This program is distributed in the hope that it will be useful,      //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of       //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
//    GNU General Public License for more details.                         //
//                                                                         //
//    You should have received a copy of the GNU General Public License    //
//    along with this program; if not, write to the Free Software          //
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA        //
//    02110-1301 USA.                                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "motd.h"

#include <klocale.h>
#include <khtml_part.h>
#include <khtmlview.h>

#include <qpushbutton.h>
#include <qlayout.h>
#include <qtextbrowser.h>

Motd::Motd(QWidget *parent)
: QDialog(parent)
{
	QPushButton *button;
	QVBoxLayout *vbox;
	QFont f;

	m_textmotd = new QTextBrowser(this);
	//m_textmotd->setGeometry(5, 5, 310, 360);

	f = QFont("Courier", 10);
	f.setFixedPitch(true);
	f.setStyleHint(QFont::TypeWriter);

	m_textmotd->setFont(f);

	m_webmotd = new KHTMLPart(this);
	m_webmotd->hide();

	button = new QPushButton(i18n("Close"), this);

	vbox = new QVBoxLayout();
	setLayout(vbox);
	vbox->addWidget(m_textmotd);
	vbox->addWidget(m_webmotd->view());
	vbox->addWidget(button);

	connect(button, SIGNAL(clicked()), SLOT(close()));

	setWindowTitle(i18n("Message Of The Day (MOTD)"));
	resize(400, 400);
}

Motd::~Motd()
{
}

void Motd::append(QString text)
{
	int v, count;
	const char *html[] =
	{
		"000000", "20ff00", "0000ff", "ffa000",
		"ff00ff", "300fff", "a0d000", "00ff70",
		"ff20ff", "777777", "AAAAAA"
	};
	QString buffer;

	count = 0;
	for(int i = 0; i < text.length(); i++)
	{
		if(text[i] != '%')
		{
			if(text[i] == ' ')
					buffer.append("&nbsp;");
			else if(text[i] == '<')
					buffer.append("&lt;");
			else if(text[i] == '>')
					buffer.append("&gt;");
			else if(text[i] == '\n')
			{
					buffer.append("<br>\n");
					count = 0;
			}
			else
					buffer.append(text[i]);

			count++;
		}
		else
		{
			i += 2;
			v = text[i].digitValue();
			if((v >= 0) && (v <= 9))
			{
				buffer.append("<font color=#");
				buffer.append(html[v]);
				buffer.append(">");

			}
		}
	}

	m_textmotd->append(buffer);
}

void Motd::setWebpage(QString url)
{
	if(url.isNull())
		return;

	m_textmotd->hide();
	m_webmotd->show();

	m_webmotd->openUrl(url);
}

void Motd::setText(QString motd)
{
	m_textmotd->setText(motd);
}

