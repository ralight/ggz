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
// KGGZChat: display the chat, receive text/events, and send user text via signal. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header definition
#include "KGGZChat.h"

// KGGZ includes
#include "KGGZCommon.h"

// KDE includes
#include <kapp.h>
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qlabel.h>

// System includes
#include <X11/Xlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fnmatch.h>

// buffer overflow??
char m_buftmp[2048];

// Constructor
KGGZChat::KGGZChat(QWidget *parent = NULL, char *name = NULL)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;
	QLabel *label;

	input = new QLineEdit(this);
	input->setFixedHeight(20);
	input->setEnabled(FALSE);

	label = new QLabel(i18n("Enter your message here:"), this);

	output = new KTextBrowser(this);

	vbox1 = new QVBoxLayout(this, 5);
	vbox1->add(output);
	vbox1->add(label);
	vbox1->add(input);

	connect(input, SIGNAL(returnPressed()), SLOT(slotSend()));

	for(int i = 0; i < 10; i++)
		lag[i].lagid = 0;

	receive(NULL, "GGZ Gaming Zone 0.0.4", RECEIVE_ADMIN);
	receive(NULL, i18n("Ready for connection..."), RECEIVE_ADMIN);

	m_listusers = 0;

	input->setFocus();
}

// Destructor
// TODO: save chat log?
KGGZChat::~KGGZChat()
{
}

// Call takes lagid and returns it if valid, else -1 (e.g. buffer full)
long KGGZChat::setLag(long lagid)
{
	if(lagid <= 0) return -1;
	KGGZDEBUG("set %lu\n", lagid);
	for(int i = 0; i < 10; i++)
	{
		if(lag[i].lagid == 0)
		{
			lag[i].lagid = lagid;
			gettimeofday(&lag[i].lagtime, NULL);
			return lagid;
		}
		KGGZDEBUG("--SET %lu on %i\n", lag[i].lagid, i);
	}
	return -1;
}

// Call takes lagid and returns time difference, or -1 on error
long KGGZChat::getLag(long lagid)
{
	struct timeval tmp;
	long ret;

	if(lagid <= 0) return -1;
	KGGZDEBUG("get %lu\n", lagid);
	for(int i = 0; i < 10; i++)
	{
		if(lag[i].lagid == lagid)
		{
			lag[i].lagid = 0;
			gettimeofday(&tmp, NULL);
			ret = ((tmp.tv_sec - lag[i].lagtime.tv_sec) * 1000 + (tmp.tv_usec - lag[i].lagtime.tv_usec) / 1000);
			KGGZDEBUG("--get FINAL %lu\n", ret);
			return ret;
		}
		KGGZDEBUG("--get %lu on %i\n", lag[i].lagid, i);
	}
	return -1;
}

// TODO: real random()ized lag
long KGGZChat::randomLag()
{
	static long x;

	x += 247668;
	return x;
}

// Send out a message or execute command
void KGGZChat::slotSend()
{
	enum Events {EVENT_JOIN, EVENT_CHAT, EVENT_LIST, EVENT_WHO, EVENT_BEEP, EVENT_LAG, EVENT_ME, EVENT_HELP, EVENT_NOEVENT, EVENT_LOCAL};
	char *commands = NULL;
	char *op1 = NULL;
	char *preop = NULL;
	char *preop2 = NULL;
	int triggerevent;
	char *inputtext;
	float fahrenheit = 0.0, celsius = 0.0, reaumour = 0.0, kelvin = 0.0;

	if(strlen(input->text()) == 0) return;

        triggerevent = EVENT_CHAT;
        inputtext = (char*)malloc(strlen(input->text()) + 1);
        strcpy(inputtext, input->text());

        if(inputtext[0] == '/')
        {
		if(strncmp(inputtext, "/me", 3) == 0) triggerevent = EVENT_ME;
                else
		{
			triggerevent = EVENT_NOEVENT;
			commands = strtok(inputtext, " ");
		}
		if(strcmp(commands, "/local") == 0)
		{
			preop = strtok(NULL, " ");
			preop2 = strtok(NULL, " ");
			if((preop) && (preop2) && (strlen(preop2) == 1))
			{
				triggerevent = EVENT_LOCAL;
				kelvin = 0.0;
				switch(*preop2)
				{
					case 'F':
					case 'f': // Fahrenheit
						fahrenheit = atof(preop);
						celsius = (fahrenheit - 32.0) / 1.8;
						reaumour = ((fahrenheit - 32.0) / 1.8) * 0.8;
						kelvin = (fahrenheit - 32.0) / 1.8 - 273.15;
						break;
					case 'C':
					case 'c': // Celsius
						celsius = atof(preop);
						reaumour = celsius * 0.8;
						kelvin = celsius - 273.15;
						fahrenheit = (celsius * 1.8) + 32.0;
						break;
					case 'R':
					case 'r': // Reaumour
						reaumour = atof(preop);
						celsius = reaumour / 0.8;
						kelvin = (reaumour / 0.8) - 273.15;
						fahrenheit = ((reaumour / 0.8) * 1.8) + 32.0;
						break;
					case 'K':
					case 'k': // Kelvin
						kelvin = atof(preop);
						celsius = kelvin - 273.15;
						reaumour = (kelvin - 273.15) / 0.8;
						fahrenheit = (kelvin - 273.15) + 32.0;
						break;
				}
				op1 = (char*)malloc(512);
				if(kelvin != 0.0)
					sprintf(op1, i18n("Temperature: %3.1f°F, %3.1f°C, %3.1f°R %3.1f°K"), fahrenheit, celsius, reaumour, kelvin);
				else
					sprintf(op1, i18n("Usage of local: /local &lt;number&gt; &lt;unit&gt;, with unit being one of C, R, K, F"));
			}
		}
                if(strcmp(commands, "/join") == 0)
                {
                        triggerevent = EVENT_JOIN;
                        preop = strtok(NULL, " ");
                        op1 = (char*)malloc(strlen(preop) + 1);
                        sprintf(op1, "%s", preop);
                }
                if(strcmp(commands, "/list") == 0) triggerevent = EVENT_LIST;
                if(strcmp(commands, "/who") == 0) triggerevent = EVENT_WHO;
		if(strcmp(commands, "/help") == 0) triggerevent = EVENT_HELP;
		if(strcmp(commands, "/?") == 0) triggerevent = EVENT_HELP;
		if(strcmp(commands, "/lag") == 0)
		{
			op1 = (char*)malloc(128);
			sprintf(op1, "/me tests his lag (LAGID %lu)", setLag(randomLag()));
			triggerevent = EVENT_LAG;
		}
		if(strcmp(commands, "/beep") == 0)
		{
                        preop = strtok(NULL, " ");
                        op1 = (char*)malloc(strlen(preop) + 1);
			sprintf(op1, "%s", preop);
			if(strlen(op1) > 0) triggerevent = EVENT_BEEP;
		}
		//while(commands != NULL) strtok(commands, " "); // ? is this necessary?
        }

        switch(triggerevent)
        {
		case EVENT_HELP:
			receive(NULL, i18n("KGGZ - The KDE client for the GGZ Gaming Zone"), RECEIVE_ADMIN);
			receive(NULL, i18n("List of available commands:"), RECEIVE_ADMIN);
			receive(NULL, i18n("/me &lt;msg&gt; - Special messages"), RECEIVE_ADMIN);
			receive(NULL, i18n("/lag - Test connection speed"), RECEIVE_ADMIN);
			receive(NULL, i18n("/local &lt;information&gt; - l10n operation"), RECEIVE_ADMIN);
			receive(NULL, i18n("/beep &lt;name&gt; - Makes name's computer beep"), RECEIVE_ADMIN);
			receive(NULL, i18n("/help - This help screen (also /?)"), RECEIVE_ADMIN);
			receive(NULL, i18n("Additional:"), RECEIVE_ADMIN);
			receive(NULL, i18n("/join &lt;room no&gt; - Join a room"), RECEIVE_ADMIN);
			receive(NULL, i18n("/list - List room names"), RECEIVE_ADMIN);
			receive(NULL, i18n("/who - List player names in room"), RECEIVE_ADMIN);
			break;
                case EVENT_WHO:
			//ggzcore_event_enqueue(GGZ_PLAYER_LIST, NULL, NULL);
			//ggzcore_room_list_players(KGGZ_Server::currentRoom(KGGZ_Server::currentServer()));
			m_listusers = 1;
                        break;
                case EVENT_LIST:
			//ggzcore_event_enqueue(GGZ_ROOM_LIST, NULL, NULL);
			//ggzcore_server_list_rooms(KGGZ_Server::currentServer(), -1, 1);
                        break;
                case EVENT_JOIN:
			//ggzcore_event_enqueue(GGZ_ROOM_ENTER, (void*)atoi(op1), NULL);
			//ggzcore_server_join_room(KGGZ_Server::currentServer(), atoi(op1));
                        break;
                case EVENT_CHAT:
		case EVENT_ME:
			// hey boy, this is history!
	                // ggzcore_event_enqueue(GGZ_CHAT_NORMAL, strdup(inputtext), free);
			// ggzcore_room_chat(KGGZ_Server::currentRoom(KGGZ_Server::currentServer()), GGZ_CHAT_NORMAL, NULL, strdup(inputtext));
			emit signalChat(inputtext);
                        break;
                case EVENT_BEEP:
			//ggzcore_event_enqueue(GGZ_CHAT_BEEP, strdup(op1), free);
			//ggzcore_room_chat(KGGZ_Server::currentRoom(KGGZ_Server::currentServer()), GGZ_CHAT_BEEP, strdup(op1), NULL);
                        break;
		case EVENT_LAG:
			//ggzcore_event_enqueue(GGZ_CHAT_PERSONAL, strdup(op1), free);
			//ggzcore_room_chat(KGGZ_Server::currentRoom(KGGZ_Server::currentServer()), GGZ_CHAT_PERSONAL, , strdup(op1));  -- FIXME !!!
			break;
		case EVENT_LOCAL:
			receive(NULL, op1, RECEIVE_ADMIN);
			break;
                default:
			//ggzcore_event_enqueue(GGZ_USER_CHAT, strdup("Error! Unknown command!"), free);
			receive(NULL, i18n("Error! Unknown command! Try /help instead."), RECEIVE_ADMIN);
        }
	input->clear();

        if((triggerevent == EVENT_JOIN) || (triggerevent == EVENT_BEEP) || (triggerevent == EVENT_LAG) || (triggerevent = EVENT_LOCAL))
        {
                if(op1) free(op1);
        }
        free(inputtext);
}

// Determines whether to accept a character as delimiter
int KGGZChat::separator(const char *c)
{
	if((*c == ' ') || ((*c == '.') && (*(c + 1) == ' ')) || (*c == ',')) return 1;
	return 0;
}

// Returns the plain text of the HTML input
char *KGGZChat::plaintext(const char *text)
{
	static char *ret = NULL;
	int j = 0;

	if(ret) free(ret);
	ret = (char*)malloc(strlen(text) * 3); // erm... :-)
	strcpy(ret, "");
	for(int i = 0; i < strlen(text); i++)
	{
		if(text[i] == '<')
		{
			strcat(ret, "&lt;");
			j += 4;
		}
		else if(text[i] == '>')
		{
			strcat(ret, "&gt;");
			j += 4;
		}
		else
		{
			ret[j] = text[i];
			ret[j + 1] = 0;
			j++;
		}
	}
	ret[j] = 0;
	return ret;
}

// Receives text queue, URL highlighting
void KGGZChat::parse(char *text)
{
	int i, j, k, kiv, flag, n;
	char localbuf[16384];
	char tmpbuf[1024];
	char tmpbufiv[1024]; // invisible
	char *c;

	i = 0;
	j = 0;
	k = 0;
	kiv = 0;
	n = 0;
	//KGGZ_Server::loop(); not necessary anymore here.

	KGGZDEBUG("Parse-pre: %s\n", text);
	if(!text) return;

        KGGZDEBUG("Parse: %s\n", text);
	strcpy(localbuf, "");
	for(i = 0; i < strlen(text); i++)
	{
		flag = 0;
		c = text + i;
		if(c[0] == '@')
		{
			k = i;
			while((text[k] != ' ') && (k < strlen(text)))
			{
				if((text[k] == '.') && (k - i > 2))
				{
					if((separator(&text[k + 3])) || (separator(&text[k + 4]))) flag = 1;
				}
				k++;
			}
			if(flag)
			{
				while((text[i] != ' ') && (i > 0))
				{
					i--;
					j--;
				}
				i++;
				j++;
				strcpy(tmpbuf, "");
				strcpy(tmpbufiv, "mailto:");
				k = strlen(tmpbuf);
				kiv = strlen(tmpbufiv);
				localbuf[j] = 0;
				strcat(localbuf, "<A HREF=\"");
				n = 0;
				while(i < strlen(text))
				{
					/*if(text[i] == '@') n = 1;
					if(n > 0)
					{
						if(separator(text[i])) n++;
						if((text[i] == '.') && (n < 10)) n = 11;
					}
					if((n % 10) == 2) break;*/
					if(separator(&text[i])) n++;
					if(n == 1) break;
					tmpbuf[k] = text[i];
					tmpbufiv[kiv] = text[i];
					i++;
					k++;
					kiv++;
				}
				tmpbuf[k] = 0;
				tmpbufiv[kiv] = 0;
				strcat(localbuf, tmpbufiv);
				strcat(localbuf, "\">");
				strcat(localbuf, tmpbuf);
				strcat(localbuf, "</A>");
				j += strlen(tmpbuf) + strlen(tmpbufiv) + 15;
				KGGZDEBUG("eDebug(1): |%s|\n", tmpbuf);
				KGGZDEBUG("eDebug(2): |%s|\n", tmpbufiv);
				KGGZDEBUG("eDebug(3): |%s|\n", localbuf);
				flag = 0;
			}
		}
		if((c[0] == 'h') && (c[1] == 't') && (c[2] == 't') && (c[3] == 'p') && (c[4] == ':')) flag = 5;
		if((c[0] == 'w') && (c[1] == 'w') && (c[2] == 'w') && (c[3] == '.')) flag = 4;
		if((c[0] == 'g') && (c[1] == 'g') && (c[2] == 'z') && (c[3] == ':')) flag = 14;
		if((c[0] == 'm') && (c[1] == 'a') && (c[2] == 'i') && (c[3] == 'l') && (c[4] == 't') && (c[5] == 'o') && (c[6] == ':')) flag = 7;
		if(flag)
		{
			localbuf[j] = 0;
			strcat(localbuf, "<A HREF=\"");
			i += flag % 10;
			switch(flag)
			{
				case 4:
					strcpy(tmpbuf, "www.");
					strcpy(tmpbufiv, "http://www.");
					break;
				case 5:
					sprintf(tmpbuf, "http:");
					strcpy(tmpbufiv, tmpbuf);
					break;
				case 7:
					strcpy(tmpbuf, "mailto:");
					strcpy(tmpbufiv, tmpbuf);
					break;
				case 14:
					strcpy(tmpbuf, "ggz:");
					strcpy(tmpbufiv, tmpbuf);
					break;
			}
			k = strlen(tmpbuf);
			kiv = strlen(tmpbufiv);
			while((!separator(&text[i])) && (i < strlen(text)))
			{
				tmpbuf[k] = text[i];
				tmpbufiv[kiv] = text[i];
				k++;
				kiv++;
				i++;
			}
			tmpbuf[k] = 0;
			tmpbufiv[kiv] = 0;
			strcat(localbuf, tmpbufiv);
			strcat(localbuf, "\">");
			strcat(localbuf, tmpbuf);
			strcat(localbuf, "</A>");
			j += strlen(tmpbuf) + strlen(tmpbufiv) + 16;
			localbuf[j - 1] = text[i]; // oh this may cause real damage :-)
			KGGZDEBUG("Debug(1): |%s|\n", tmpbuf);
			KGGZDEBUG("Debug(2): |%s|\n", tmpbufiv);
			KGGZDEBUG("Debug(3): |%s|\n", localbuf);
		}
		else
		{
			localbuf[j] = text[i];
			j++;
		}
	}
	localbuf[j] = 0;
	strcat(localbuf, "<br>");
	KGGZDEBUG("Debug(4): |%s|\n", localbuf);
	output->setText(output->text() + localbuf);
	output->setContentsPos(0, 32767);
	logChat(QString(localbuf));
}

// Checks whether Lag test has been issued
void KGGZChat::checkLag(const char *text)
{
	int i, k;
	const char *c;
	char tmpbuf[128];
	long ms;

	if(!text) return;
	for(i = 0; i < strlen(text); i++)
	{
		c = text + i;
		if((c[0] == 'L') && (c[1] == 'A') && (c[2] == 'G') && (c[3] == 'I') && (c[4] == 'D'))
		{
			KGGZDEBUG("Recognized Lag-ID!\n");
			k = 6;
			while((c[k] >= '0') && (c[k] <= '9'))
			{
				tmpbuf[k - 6] = c[k];
				k++;
			}
			tmpbuf[k - 6] = 0;
			KGGZDEBUG("**tmpbuf is %s\n", tmpbuf);
			ms = getLag(atoll(tmpbuf)); // transformed to store it somewhere
			if(ms >= 0)
				sprintf(tmpbuf, "/me does now know his lag (%li milliseconds)", ms);
			else
				sprintf(tmpbuf, "/me has not issued this lag-id!");
			//ggzcore_event_enqueue(GGZ_CHAT, strdup(tmpbuf), free);
			//ggzcore_room_chat(KGGZ_Server::currentRoom(KGGZ_Server::currentServer()), GGZ_CHAT_NORMAL, NULL, strdup(tmpbuf));
		}
	}
}

// Log chat messages
// FIXME: only when user has logging enabled
// FIXME 2: append with date !!!
void KGGZChat::logChat(QString text)
{
	FILE *f;
	//char s[1024];
	QString s;

	//strcpy(s, getenv("HOME"));
	//strcat(s, "/.ggz/kggzlog.html");
	s.append(getenv("HOME"));
	s.append("/.ggz/kggzlog.html");

	f = fopen(s.latin1(), "a");
	if(f)
	{
		fprintf(f, "%s\n", text.latin1());
		fclose(f);
	}
}

// receive chat and admin messages
void KGGZChat::receive(const char *player, const char *message, ReceiveMode mode)
{
	QString tmp;

	KGGZDEBUG("Receiving: %s (%i)\n", message, mode);
	switch(mode)
	{
		case RECEIVE_CHAT:
			tmp = QString("<font color=#0000ff>") + QString(player) + QString(":&nbsp;&nbsp;</font>");
			output->append(tmp);
			logChat(tmp);
			parse(plaintext(message));
			break;
		case RECEIVE_OWN:
			tmp = QString("<font color=#0000ff><b>") + QString(player) + QString("</b>:&nbsp;&nbsp;</font>");
			output->setText(output->text() + tmp);
			logChat(tmp);
			parse(plaintext(message));
			break;
		case RECEIVE_ADMIN:
			tmp = QString("<font color=#ff0000>") + QString(message) + QString("</font><br>");
			output->setText(output->text() + tmp);
			output->setContentsPos(0, 32767);
			logChat(tmp);
			break;
		case RECEIVE_INFO:
			tmp = QString("<font color=#00a000><i>* ") + QString(player) + QString(message + 3) + QString("</i></font><br>");
			output->setText(output->text() + tmp);
			output->setContentsPos(0, 32767);
			logChat(tmp);
			checkLag(tmp);
			break;
		default:
			KGGZDEBUG("ERROR! Unknown chat mode\n");
	}
}

// initialize the chat (enable it)
void KGGZChat::init()
{
	input->setEnabled(TRUE);
	input->setFocus();
}

// execute a beep event
void KGGZChat::beep()
{
	XBell(kapp->getDisplay(), 100);
}

// disable the chat
void KGGZChat::shutdown()
{
	input->setEnabled(FALSE);
}
