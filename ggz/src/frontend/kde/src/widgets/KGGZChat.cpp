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
// KGGZChat: display the chat, receive text/events, and send user text via signal. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header definition
#include "KGGZChat.h"

// KGGZ includes
#include "KGGZCommon.h"

// KDE includes
#include <kapplication.h>
#include <klocale.h>
#include <kprocess.h>

// Qt includes
#include <qlayout.h>
#include <qlabel.h>

// System includes
//#include <X11/Xlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fnmatch.h>
#include <time.h>

// Constructor
KGGZChat::KGGZChat(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;
	QLabel *label;

	m_listusers = 0;
	m_log = 0;
	m_speech = 0;
	m_timestamp = 0;

	input = new KGGZChatLine(this);
	input->setFixedHeight(20);
	input->setEnabled(FALSE);

	label = new QLabel(i18n("Enter your message here:"), this);

	output = new KTextBrowser(this);

	vbox1 = new QVBoxLayout(this, 5);
	vbox1->add(output);
	vbox1->add(label);
	vbox1->add(input);

	connect(input, SIGNAL(returnPressed()), SLOT(slotSend()));

	receive(NULL, "GGZ Gaming Zone " KGGZVERSION, RECEIVE_INFO);
	receive(NULL, i18n("Ready for connection..."), RECEIVE_INFO);

	input->setFocus();
}

// Destructor
// TODO: save chat log?
KGGZChat::~KGGZChat()
{
}

void KGGZChat::setLogging(int log)
{
	m_log = log;
}

void KGGZChat::setSpeech(int speech)
{
	m_speech = speech;
}

void KGGZChat::setTimestamp(int timestamp)
{
	m_timestamp = timestamp;
}

// Send out a message or execute command
void KGGZChat::slotSend()
{
	enum Events
	{
		EVENT_CHAT,
		EVENT_BEEP,
		EVENT_ME,
		EVENT_NOEVENT,
		EVENT_PERSONAL,
		EVENT_HELP,
		EVENT_MARK,
		EVENT_AWAY,
		EVENT_ANNOUNCE,
		EVENT_TABLE
	};
	char *commands = NULL;
	char *op2 = NULL;
	int triggerevent;
	char *inputtext;
	char *inputargs;
	char *player = NULL;
	char *timestring;
	time_t curtime;

	if(strlen(input->text()) == 0) return;

	triggerevent = EVENT_CHAT;
	inputtext = (char*)malloc(strlen(input->text()) + 1);
	inputargs = (char*)malloc(strlen(input->text()) + 1);
	strcpy(inputtext, input->text());
	strcpy(inputargs, "");

	if(inputtext[0] == '/')
	{
		if(strcmp(inputtext, "/help") == 0) triggerevent = EVENT_HELP;
		if(strcmp(inputtext, "/mark") == 0) triggerevent = EVENT_MARK;
		if(strncmp(inputtext, "/me", 3) == 0) triggerevent = EVENT_ME;
		if(triggerevent == EVENT_CHAT)
		{
			triggerevent = EVENT_NOEVENT;
			commands = strtok(inputtext, " ");
			if(commands)
			{
				if(strncmp(inputtext, "/msg", 4) == 0)
				{
					KGGZDEBUG("--private message--\n");
					if(commands) player = strtok(NULL, " ");
					if(player)
					{
						triggerevent = EVENT_PERSONAL;
						op2 = strtok(NULL, " ");
						if(op2) strcat(inputargs, op2);
					}
					while(op2)
					{
						op2 = strtok(NULL, " ");
						if(op2)
						{
							strcat(inputargs, " ");
							strcat(inputargs, op2);
						}
					}
				}
				if(strncmp(commands, "/table", 6) == 0)
				{
					triggerevent = EVENT_TABLE;
					op2 = strtok(NULL, " ");
					while(op2)
					{
						strcat(inputargs, " ");
						strcat(inputargs, op2);
						op2 = strtok(NULL, " ");
					}
					KGGZDEBUG("---table chart %s---\n", inputargs);
				}
				if(strncmp(inputtext, "/wall", 5) == 0)
				{
					KGGZDEBUG("--announcement message--\n");
					triggerevent = EVENT_ANNOUNCE;
					op2 = strtok(NULL, " ");
					while(op2)
					{
						strcat(inputargs, " ");
						strcat(inputargs, op2);
						op2 = strtok(NULL, " ");
					}
				}
				if(strcmp(commands, "/beep") == 0)
				{
					KGGZDEBUG("--beep message--\n");
					if(commands) player = strtok(NULL, " ");
					if(player)
					{
						triggerevent = EVENT_BEEP;
						op2 = strtok(NULL, " ");
					}
					while(op2)
					{
						op2 = strtok(NULL, " ");
					}
				}
				if(strcmp(commands, "/away") == 0)
				{
					triggerevent = EVENT_AWAY;
					KGGZDEBUG("--away message--\n");
					if(commands) op2 = strtok(NULL, " ");
					if(op2) strcat(inputargs, op2);
					while(op2)
					{
						op2 = strtok(NULL, " ");
						if(op2)
						{
							strcat(inputargs, " ");
							strcat(inputargs, op2);
						}
					}
				}
			}
		}
	}

	switch(triggerevent)
	{
		case EVENT_HELP:
			receive(NULL, i18n("KGGZ - The KDE client for the GGZ Gaming Zone"), RECEIVE_INFO);
			receive(NULL, i18n("List of available commands:"), RECEIVE_INFO);
			receive(NULL, i18n("/me &lt;msg&gt; - sends an emphasized phrase."), RECEIVE_INFO);
			receive(NULL, i18n("/msg &lt;player&gt; &lt;message&gt; - sends a private message."), RECEIVE_INFO);
			receive(NULL, i18n("/table &lt;message&gt; - table message."), RECEIVE_INFO);
			receive(NULL, i18n("/wall &lt;message&gt; - sends a message to all rooms."), RECEIVE_INFO);
			receive(NULL, i18n("/beep &lt;player&gt; - send player a beep."), RECEIVE_INFO);
			receive(NULL, i18n("/help - this screen."), RECEIVE_INFO);
			receive(NULL, i18n("/mark - mark a time stamp."), RECEIVE_INFO);
			receive(NULL, i18n("/away &lt;message&gt; - show a goodbye message."), RECEIVE_INFO);
			break;
		case EVENT_CHAT:
			emit signalChat(inputtext, NULL, RECEIVE_CHAT);
			break;
		case EVENT_TABLE:
			emit signalChat(inputargs, NULL, RECEIVE_TABLE);
			break;
		case EVENT_ANNOUNCE:
			emit signalChat(inputargs, NULL, RECEIVE_ANNOUNCE);
			break;
		case EVENT_ME:
			emit signalChat(inputtext, NULL, RECEIVE_CHAT);
			break;
		case EVENT_PERSONAL:
			if(player)
			{
				emit signalChat(inputargs, player, RECEIVE_PERSONAL);
				//receive(NULL, i18n("Sent message to %1.").arg(player), RECEIVE_ADMIN);
				receive(i18n("--> %1").arg(player), inputargs, RECEIVE_PERSONAL);
			}
			break;
		case EVENT_BEEP:
			if(player)
			{
				emit signalChat(inputtext, player, RECEIVE_BEEP);
				receive(NULL, i18n("Beeped Player %1.").arg(player), RECEIVE_ADMIN);
			}
			break;
		case EVENT_MARK:
			curtime = time(NULL);
			timestring = ctime(&curtime);
			timestring[strlen(timestring) - 1] = 0;
			KGGZDEBUG("timestring: %s\n", timestring);
			//strcat(inputargs, timestring);
			//strcat(inputargs, i18n(" -- MARK --"));
			//KGGZDEBUG("send out: %s\n", inputargs);
			receive(NULL, i18n("%1 -- MARK --").arg(timestring), RECEIVE_ADMIN);
			break;
		case EVENT_AWAY:
			if((inputargs) && (strlen(inputargs) > 0))
				emit signalChat(i18n("/me is going to leave (%1)").arg(inputargs), NULL, RECEIVE_CHAT);
			else
				emit signalChat(i18n("/me has returned"), NULL, RECEIVE_CHAT);
			break;
		default:
			receive(NULL, i18n("Error! Unknown command! Try /help instead."), RECEIVE_ADMIN);
	}

	input->clear();

	free(inputtext);
	free(inputargs);
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
	ret = (char*)malloc(strlen(text) * 7); // erm... :-)
	strcpy(ret, "");
	for(int i = 0; i < (int)strlen(text); i++)
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
		else if((text[i] == ' ') && (i < 60))
		{
			// quick hack
			strcat(ret, "&nbsp;");
			j += 6;
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
QString KGGZChat::parse(char *text)
{
	int i, j, k, kiv, flag, n;
	char localbuf[16384];
	char tmpbuf[1024];
	char tmpbufiv[1024]; // invisible
	char *c;
	QString ret;

	i = 0;
	j = 0;
	k = 0;
	kiv = 0;
	n = 0;
	//KGGZ_Server::loop(); not necessary anymore here.

	KGGZDEBUG("Parse-pre: %s\n", text);
	if(!text) return QString::null;

	KGGZDEBUG("Parse: %s\n", text);
	strcpy(localbuf, "");
	for(i = 0; i < (int)strlen(text); i++)
	{
		flag = 0;
		c = text + i;
		if(c[0] == '@')
		{
			k = i;
			while((text[k] != ' ') && (k < (int)strlen(text)))
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
				while(i < (int)strlen(text))
				{
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
			while((!separator(&text[i])) && (i < (int)strlen(text)))
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
	KGGZDEBUG("Debug(4): |%s|\n", localbuf);
	ret = QString("%1</td></tr>").arg(localbuf);
	return ret;
}

// Log chat messages
void KGGZChat::logChat(QString text)
{
	FILE *f;
	QString s;

	if(!m_log) return;

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
	static QString lastplayer;
	QString color;
	QString msg;
	char timestring[32];
	int ret;
	time_t curtime;

	switch(mode)
	{
		case RECEIVE_CHAT:
			color = "0000ff";
			break;
		case RECEIVE_TABLE:
			color = "9999ff";
			break;
		case RECEIVE_ANNOUNCE:
			color = "80d000";
			break;
		case RECEIVE_ME:
			color = "00a000";
			break;
		case RECEIVE_OWN:
			color = "0000ff";
			break;
		case RECEIVE_PERSONAL:
			color = "b0b000";
			break;
		default:
			break;
	}

	msg = msg.fromUtf8(message);

	if((lastplayer == player) && (mode == RECEIVE_CHAT)) color = "ffffff";
	else lastplayer = player;

	if(m_speech)
	{
		KProcess *proc = new KProcess();
		*proc << "say" << msg.latin1();
		proc->start();
	}

	if(m_timestamp)
	{
		curtime = time(NULL);
		ret = strftime(timestring, sizeof(timestring), "%H:%M:%S &nbsp; ", localtime(&curtime));
		if(!ret) timestring[0] = 0;
	}
	else timestring[0] = 0;


	KGGZDEBUG("Receiving: %s (%i)\n", msg.latin1(), mode);
	switch(mode)
	{
		case RECEIVE_CHAT:
			tmp = QString("<tr><td>%1<font color=#%2>").arg(timestring).arg(color) + QString(player) + QString(":&nbsp;</font></td><td>");
			tmp += parse(plaintext(msg.latin1()));
			output->append(tmp);
			logChat(tmp);
			break;
		case RECEIVE_TABLE:
			tmp = QString("<tr><td>%1<font color=#%2>").arg(timestring).arg(color) + QString(player) + QString(":&nbsp;</font></td><td>");
			tmp += parse(plaintext(msg.latin1()));
			output->append(tmp);
			logChat(tmp);
			break;
		case RECEIVE_OWN:
			tmp = QString("<tr><td>%1<font color=#%2><b>").arg(timestring).arg(color) + QString(player) + QString("</b>:&nbsp;</font></td><td>");
			tmp += parse(plaintext(msg.latin1()));
			output->append(tmp);
			logChat(tmp);
			break;
		case RECEIVE_ADMIN:
			tmp = QString("<tr><td colspan=2>%1<font color=#ff0000>").arg(timestring) + msg + QString("</font></td></tr>");
			output->append(tmp);
			logChat(tmp);
			break;
		case RECEIVE_INFO:
			tmp = QString("<tr><td colspan=2>%1<font color=#802020>").arg(timestring) + msg + QString("</font></td></tr>");
			output->append(tmp);
			logChat(tmp);
			break;
		case RECEIVE_ANNOUNCE:
			tmp = QString("<tr><td colspan=2>%1<font color=#%2><i>* ").arg(timestring).arg(color) + QString(player) + msg + QString("</i></font></td></tr>");
			output->append(tmp);
			logChat(tmp);
			break;
		case RECEIVE_ME:
			tmp = QString("<tr><td colspan=2>%1<font color=#%2><i>* ").arg(timestring).arg(color) + QString(player) + msg.right(msg.length() - 3) + QString("</i></font></td></tr>");
			output->append(tmp);
			logChat(tmp);
			break;
		case RECEIVE_PERSONAL:
			tmp = QString("<tr><td>%1<font color=#%2><b><i>").arg(timestring).arg(color);
			if(player) tmp += QString(player) + ":&nbsp;</i></b></font><font color=#b0b000><b><i>";
			tmp += QString("</td><td>") + QString(plaintext(msg.latin1())) + QString("</i></b></font></td></tr>");
			output->append(tmp);
			logChat(tmp);
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
	input->setFocusPolicy(KGGZChatLine::StrongFocus);
}

// execute a beep event
void KGGZChat::beep()
{
	//XBell(kapp->getDisplay(), 100);
	kapp->beep();
}

// disable the chat
void KGGZChat::shutdown()
{
	input->clear();
	input->setEnabled(FALSE);
}

KGGZChatLine *KGGZChat::chatline()
{
	return input;
}

