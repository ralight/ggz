/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4           //
// Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net //
// The MindX Open Source Project - http://mindx.sourceforge.net            //
// Published under GNU GPL conditions - view COPYING for details           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

// Header definition
#include "KGGZMotd.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qpushbutton.h>

// System includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// KGGZ includes
#include "KGGZCommon.h"

// Constructor
KGGZMotd::KGGZMotd(QWidget *parent = NULL, char *name = NULL)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QPushButton *button;

	edit = new QTextView(this);
	edit->setGeometry(5, 5, 310, 360);
	edit->setFont(QFont("Courier", 8, QFont::Bold));

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
	char buf[1024];

	KGGZDEBUGF("KGGZMotd::append(%s)\n", text);
	KGGZDEBUGF("text length: %i\n", strlen(text));
	count = 0;
	strcpy(buf, "");
	for(i = 0; i < strlen(text); i++)
	{
		if(text[i] != '%')
		{
			switch(text[i])
			{
				case ' ':
					sprintf(tmp, "&nbsp;");
					break;
				case '<':
					sprintf(tmp, "&lt;");
					break;
				case '>':
					sprintf(tmp, "&gt;");
					break;
				default:
					sprintf(tmp, "%c", text[i]);
			}
			strcat(buf, tmp);
			count++;
			if(count > 45)
			{
				count = 0;
				strcat(buf, "<br>");
			}
		}
		else
		{
			i+=2;
			j = (int)text[i] - 48;
			if((j >= 0) && (j <= 9))
			{
				strcat(buf, "<font color=#");
				strcat(buf, html[j]);
				strcat(buf, ">");
			}
		}
	}
	KGGZDEBUGF("append text now!\n");
	// This Qt bug is solved in the current CVS snapshot.
	//edit->append(buf);
	edit->setText(edit->text() + buf);
}

void KGGZMotd::setSource(void *data)
{
	char **motd;

	KGGZDEBUGF("KGGZMotd::setSource()\n");
	edit->setText("GGZ Gaming Zone Message Of The Day:<BR><BR>");

	KGGZDEBUG("--> cleared\n");
	motd = (char**)data;
	KGGZDEBUG("--> casted\n");
	if(!motd)
	{
		KGGZDEBUG("No MOTD data received!\n");
		return;
	}
	KGGZDEBUG("--> checked\n");
	for(int i = 0; motd[i] != NULL; i++)
	{
		KGGZDEBUG("--> append: %s\n", motd[i]);
		append(motd[i]);
	}
}
