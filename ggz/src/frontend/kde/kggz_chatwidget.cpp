#include "kggz_chatwidget.h"
#include "kggz_server.h"
#include <qlayout.h>
#include <qlabel.h>
#include <string.h>
#include <stdlib.h>
#include <qstring.h>
#include <stdio.h>
#include <fnmatch.h>
#include <sys/time.h>
#include <klocale.h>

int xlag;
struct Lag
{
	long lagid;
	struct timeval lagtime;
};
Lag lag[10];

// buffer overflow!
char m_buffer[8192];
char m_buftmp[2048];

KGGZ_Chatwidget::KGGZ_Chatwidget(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;
	QLabel *label;

	input = new QLineEdit(this);
	input->setFixedHeight(20);

	label = new QLabel(i18n("Enter your message here:"), this);

	output = new KTextBrowser(this);

	vbox1 = new QVBoxLayout(this, 0);
	vbox1->add(output);
	vbox1->add(label);
	vbox1->add(input);

	connect(input, SIGNAL(returnPressed()), SLOT(send()));

	sprintf(m_buffer, "");
	for(int i = 0; i < 10; i++)
		lag[i].lagid = 0;

	adminreceive("GNU Gaming Zone 0.0.4");
	adminreceive(i18n("Ready for connection..."));

	startTimer(200);
}

KGGZ_Chatwidget::~KGGZ_Chatwidget()
{
}

// call takes lagid and returns it if valid, else -1 (e.g. buffer full)
long setlag(long lagid)
{
	printf("set %lu\n", lagid);
	for(int i = 0; i < 10; i++)
	{
		if(lag[i].lagid == 0)
		{
			lag[i].lagid = lagid;
			gettimeofday(&lag[i].lagtime, NULL);
			return lagid;
		}
		printf("--SET %lu\n", lag[i].lagid);
	}
	return -1;
}

// call takes lagid and returns time difference, or -1 on error
long getlag(long lagid)
{
	struct timeval tmp;
	long ret;

	printf("get %lu\n", lagid);
	for(int i = 0; i < 10; i++)
	{
		if(lag[i].lagid == lagid)
		{
			lag[i].lagid = 0;
			gettimeofday(&tmp, NULL);
			ret = ((tmp.tv_sec - lag[i].lagtime.tv_sec) * 1000 + (tmp.tv_usec - lag[i].lagtime.tv_usec) / 1000);
			printf("--get FINAL %lu\n", ret);
			return ret;
		}
		printf("--get %lu\n", lag[i].lagid);
	}
	return -1;
}

// TODO: real random()ized lag
long randomlag()
{
	static long x;

	x += 247668;
	return x;
}

void KGGZ_Chatwidget::send()
{
        enum Events {EVENT_JOIN, EVENT_CHAT, EVENT_LIST, EVENT_WHO, EVENT_BEEP, EVENT_LAG, EVENT_ME, EVENT_HELP, EVENT_NOEVENT, EVENT_LOCAL};
        char *commands;
        char *op1 = NULL;
        char *preop = NULL;
	char *preop2 = NULL;
        int triggerevent;
        char *inputtext;
	float fahrenheit, celsius, reaumour, kelvin;

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
					sprintf(op1, "Temperature: %3.1f°F, %3.1f°C, %3.1f°R %3.1f°K", fahrenheit, celsius, reaumour, kelvin);
				else
					sprintf(op1, "Usage of local: /local &lt;number&gt; &lt;unit&gt;, with unit being one of C, R, K, F");
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
			sprintf(op1, "/me tests his lag (LAGID %lu)", setlag(randomlag()));
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
			adminreceive(i18n("KGGZ - The KDE client for the GNU Gaming Zone"));
			adminreceive(i18n("List of available commands:"));
			adminreceive(i18n("/me &lt;msg&gt; - Special messages"));
			adminreceive(i18n("/lag - Test connection speed"));
			adminreceive(i18n("/local &lt;information&gt; - l10n operation"));
			adminreceive(i18n("/beep &lt;name&gt; - Makes name's computer beep"));
			adminreceive(i18n("/help - This help screen (also /?)"));
			adminreceive(i18n("Additional:"));
			adminreceive(i18n("/join &lt;room no&gt; - Join a room"));
			adminreceive(i18n("/list - List room names"));
			adminreceive(i18n("/who - List player names in room"));
			break;
                case EVENT_WHO:
                        ggzcore_event_enqueue(GGZ_USER_LIST_PLAYERS, NULL, NULL);
                        break;
                case EVENT_LIST:
                        ggzcore_event_enqueue(GGZ_USER_LIST_ROOMS, NULL, NULL);
                        break;
                case EVENT_JOIN:
                        ggzcore_event_enqueue(GGZ_USER_JOIN_ROOM, (void*)atoi(op1), NULL);
                        break;
                case EVENT_CHAT:
		case EVENT_ME:
	                ggzcore_event_enqueue(GGZ_USER_CHAT, strdup(inputtext), free);
                        break;
                case EVENT_BEEP:
	                ggzcore_event_enqueue(GGZ_USER_CHAT_BEEP, strdup(op1), free);
                        break;
		case EVENT_LAG:
			ggzcore_event_enqueue(GGZ_USER_CHAT, strdup(op1), free);
			break;
		case EVENT_LOCAL:
			adminreceive(op1);
			break;
                default:
			//ggzcore_event_enqueue(GGZ_USER_CHAT, strdup("Error! Unknown command!"), free);
			adminreceive(i18n("Error! Unknown command! Try /help instead."));
        }
	input->clear();

        if((triggerevent == EVENT_JOIN) || (triggerevent == EVENT_BEEP) || (triggerevent == EVENT_LAG) || (triggerevent = EVENT_LOCAL))
        {
                if(op1) free(op1);
        }
        free(inputtext);
}

int separator(char *c)
{
	if((*c == ' ') || ((*c == '.') && (*(c + 1) == ' ')) || (*c == ',')) return 1;
	return 0;
}

char *plaintext(char *text)
{
	static char *ret = NULL;
	int j = 0;

	if(ret) free(ret);
	ret = (char*)malloc(strlen(text) * 3); // erm...
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

/* receives text queue, implements URL highlighting */
void KGGZ_Chatwidget::timerEvent(QTimerEvent *e)
{
	int i, j, k, kiv, flag, n;
	long ms;
	char localbuf[16384];
	char tmpbuf[1024];
	char tmpbufiv[1024]; // invisible
	char *c;

	i = 0;
	j = 0;
	k = 0;
	kiv = 0;
	n = 0;
	KGGZ_Server::loop();
	if(strlen(m_buffer) == 0) return;

	strcpy(localbuf, "");
	for(i = 0; i < strlen(m_buffer); i++)
	{
		flag = 0;
		c = m_buffer + i;
		if(c[0] == '@')
		{
			k = i;
			while((m_buffer[k] != ' ') && (k < strlen(m_buffer)))
			{
				if((m_buffer[k] == '.') && (k - i > 2))
				{
					if((separator(&m_buffer[k + 3])) || (separator(&m_buffer[k + 4]))) flag = 1;
				}
				k++;
			}
			if(flag)
			{
				while((m_buffer[i] != ' ') && (i > 0))
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
				while(i < strlen(m_buffer))
				{
					/*if(m_buffer[i] == '@') n = 1;
					if(n > 0)
					{
						if(separator(m_buffer[i])) n++;
						if((m_buffer[i] == '.') && (n < 10)) n = 11;
					}
					if((n % 10) == 2) break;*/
					if(separator(&m_buffer[i])) n++;
					if(n == 1) break;
					tmpbuf[k] = m_buffer[i];
					tmpbufiv[kiv] = m_buffer[i];
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
				printf("eDebug(1): |%s|\n", tmpbuf);
				printf("eDebug(2): |%s|\n", tmpbufiv);
				printf("eDebug(3): |%s|\n", localbuf);
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
			while((!separator(&m_buffer[i])) && (i < strlen(m_buffer)))
			{
				tmpbuf[k] = m_buffer[i];
				tmpbufiv[kiv] = m_buffer[i];
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
			localbuf[j - 1] = m_buffer[i]; // oh this may cause real damage :-)
			printf("Debug(1): |%s|\n", tmpbuf);
			printf("Debug(2): |%s|\n", tmpbufiv);
			printf("Debug(3): |%s|\n", localbuf);
		}
		else
		{
			localbuf[j] = m_buffer[i];
			j++;
			if((c[0] == 'L') && (c[1] == 'A') && (c[2] == 'G') && (c[3] == 'I') && (c[4] == 'D'))
			{
				printf("Recognized Lag-ID!\n");
				k = 6;
				while((c[k] >= '0') && (c[k] <= '9'))
				{
					tmpbuf[k - 6] = c[k];
					k++;
				}
				tmpbuf[k - 6] = 0;
				printf("**tmpbuf is %s\n", tmpbuf);
				ms = getlag(atoll(tmpbuf)); // transformed to store it somewhere
				if(ms >= 0)
					sprintf(tmpbuf, "/me does now know his lag (%i milliseconds)", ms);
				else
					sprintf(tmpbuf, "/me has not issued this lag-id!");
				ggzcore_event_enqueue(GGZ_USER_CHAT, strdup(tmpbuf), free);
			}
		}
	}
	localbuf[j] = 0;
	printf("Debug(4): |%s|\n", localbuf);
	output->setText(output->text() + localbuf + QString("<br>"));
	output->setContentsPos(0, 32767);
	sprintf(m_buffer, "");
}

/* receive normal chat messages */ /* TODO: bold text on own messages */
void KGGZ_Chatwidget::receive(char *player, char *message)
{
	/* avoid long lines... */
	if(strlen(m_buffer) > 0) strcat(m_buffer, "<br>");
	/* assign new content to buffer */
	sprintf(m_buftmp, "<font color=#0000ff>%s:&nbsp;&nbsp;</font>", player);
	strcat(m_buffer, m_buftmp);
	sprintf(m_buftmp, "%s", plaintext(message));
	strcat(m_buffer, m_buftmp);
}

/* general important issues */
void KGGZ_Chatwidget::adminreceive(const char *adminmessage)
{
	/* avoid long lines... */
	if(strlen(m_buffer) > 0) strcat(m_buffer, "<br>");
	/* assign new content to buffer */
	sprintf(m_buftmp, "<font color=#ff0000>%s</font>", adminmessage);
	strcat(m_buffer, m_buftmp);
}

/* /me etc. */
void KGGZ_Chatwidget::inforeceive(const char *player, const char *infomessage)
{
	/* avoid long lines... */
	if(strlen(m_buffer) > 0) strcat(m_buffer, "<br>");
	/* assign new content to buffer */
	sprintf(m_buftmp, "<font color=#00a000><i>* %s%s</i></font>", player, infomessage + 3);
	strcat(m_buffer, m_buftmp);
}