#include "kggz_chatwidget.h"
#include "kggz_server.h"
#include <qlayout.h>
#include <qlabel.h>
#include <string.h>
#include <stdlib.h>
#include <qstring.h>
#include <stdio.h>

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

	label = new QLabel("Enter your message here:", this);

	output = new QTextView(this);
	output->append("<font color=#ff0000>GNU Gaming Zone 0.0.4</font>");
	output->append("<font color=#ff0000>Ready for connection...</font>");

	vbox1 = new QVBoxLayout(this, 0);
	vbox1->add(output);
	vbox1->add(label);
	vbox1->add(input);

	connect(input, SIGNAL(returnPressed()), SLOT(send()));

	sprintf(m_buffer, "");

	startTimer(200);
}

KGGZ_Chatwidget::~KGGZ_Chatwidget()
{
}

void KGGZ_Chatwidget::send()
{
        enum Events {EVENT_JOIN, EVENT_CHAT, EVENT_NOEVENT};
        char *commands;
        char *op1;
        char *preop;
        int triggerevent;
        char *inputtext;

        triggerevent = EVENT_CHAT;
        inputtext = (char*)malloc(strlen(input->text()) + 1);
        strcpy(inputtext, input->text());

        if(inputtext[0] == '/')
        {
                triggerevent = EVENT_NOEVENT;
                commands = strtok(inputtext, " ");
                if(strcmp(commands, "/join") == 0)
                {
                        triggerevent = EVENT_JOIN;
                        preop = strtok(NULL, " ");
                        op1 = (char*)malloc(strlen(preop) + 1);
                        sprintf(op1, "%s", preop);
                }
                //while(commands != NULL) strtok(commands, " "); // ? is this necessary?
        }

        switch(triggerevent)
        {
                case EVENT_JOIN:
                        ggzcore_event_trigger(GGZ_USER_JOIN_ROOM, (void*)atoi(op1), NULL);
                        break;
                case EVENT_CHAT:
	                ggzcore_event_trigger(GGZ_USER_CHAT, strdup(inputtext), free); //? memory leak?
                        break;
                 default:
                 ggzcore_event_trigger(GGZ_USER_CHAT, strdup("Error! Unknown command!"), free); //? memory leak?
        }
	input->clear();

        if(triggerevent == EVENT_JOIN)
        {
                free(op1);
        }
        free(inputtext);
}

void KGGZ_Chatwidget::timerEvent(QTimerEvent *e)
{
	KGGZ_Server::loop();
	if(strlen(m_buffer) != 0)
	{
		output->append(m_buffer);
		sprintf(m_buffer, "");
	}
	// TODO: /me /list /who /help...
}

/* receive normal chat messages */
void KGGZ_Chatwidget::receive(char *player, char *message)
{
	/* avoid long lines... */
	if(strlen(m_buffer) > 0) strcat(m_buffer, "<br>");
	/* assign new content to buffer */
	sprintf(m_buftmp, "<font color=#0000ff>%s:&nbsp;&nbsp;</font>", player);
	strcat(m_buffer, m_buftmp);
	sprintf(m_buftmp, "%s", message);
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
void KGGZ_Chatwidget::inforeceive(const char *infomessage)
{
	/* avoid long lines... */
	if(strlen(m_buffer) > 0) strcat(m_buffer, "<br>");
	/* assign new content to buffer */
	sprintf(m_buftmp, "<font color=#0000ff><i>%s</i></font>", infomessage);
	strcat(m_buffer, m_buftmp);
}