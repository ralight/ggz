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
	edit->setGeometry(5, 5, 290, 360);
	edit->setFont(QFont("Courier", 10, QFont::Bold));

	button = new QPushButton("OK", this);
	button->setGeometry(100, 370, 100, 20);

	connect(button, SIGNAL(clicked()), SLOT(close()));

	setCaption("MOTD");
	setFixedSize(300, 400);

	show();

	//ggzcore_event_enqueue(GGZ_USER_MOTD, NULL, NULL);
	startTimer(500);
}

KGGZ_Motd::~KGGZ_Motd()
{
}

void KGGZ_Motd::append(char *text)
{
	int i, j;
	char *tmp;
	char *html[] = {"ff0000", "00ff00", "0000ff", "ffff00", "ff00ff", "0000ff", "ffff00", "00ffff", "ff00ff", "777777", "AAAAAA"};

	for(i = 0; i < strlen(text); i++)
	{
		if(text[i] != '%')
		{
			tmp = (char*)malloc(3);
			if(text[i] == ' ') sprintf(tmp, "&nbsp;");
			else sprintf(tmp, "%c", text[i]);
			strcat(buf, tmp);
			free(tmp);
		}
		else
		{
			i++;
			if(text[i] != 'c')
			{
				tmp = (char*)malloc(4);
				sprintf(tmp, "%c%c", text[i - 1], text[i]);
				strcat(buf, tmp);
				free(tmp);
			}
			else
			{
				i++;
				j = (int)text[i] - 48;
				if((j >= 0) && (j <= 9))
				{
					strcat(buf, "<font color=#");
					strcat(buf, html[j]);
					strcat(buf, ">");
				}
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