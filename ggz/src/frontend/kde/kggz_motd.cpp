#include <ggzcore.h>
#include "kggz_motd.h"
#include <qtextbrowser.h>
#include <qpushbutton.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

QTextBrowser *edit;
char buf[16384];

KGGZ_Motd::KGGZ_Motd(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QPushButton *button;

	edit = new QTextBrowser(this);
	edit->setGeometry(5, 5, 310, 360);
	edit->setFont(QFont("Courier", 10, QFont::Bold));

	button = new QPushButton("OK", this);
	button->setGeometry(100, 370, 120, 20);

	connect(button, SIGNAL(clicked()), SLOT(close()));

	setCaption("MOTD");
	setFixedSize(320, 400);

	show();

	startTimer(500);
}

KGGZ_Motd::~KGGZ_Motd()
{
}

void KGGZ_Motd::append(char *text)
{
	int i, j, count;
	char tmp[8];
	char *html[] = {"ff0000", "00ff00", "0000ff", "ffff00", "ff00ff", "0000ff", "ffff00", "00ffff", "ff00ff", "777777", "AAAAAA"};

	count = 0;
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
	strcat(buf, "<br>");
}

void KGGZ_Motd::timerEvent(QTimerEvent *e)
{
	static int done = 0;

	if(!edit) return;
	if(done) return;

	done = 1;
	if(strlen(buf) > 0)
	{
		edit->append(buf);
		strcpy(buf, "");
	}
}