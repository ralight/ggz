#include "kggz_users.h"
#include <string.h>
#include <stdio.h>

QListViewItem *itemmain;
char userbuf[4096];
int m_lock;

KGGZ_Users::KGGZ_Users(QWidget *parent, char *name)
: QListView(parent, name)
{

	itemmain = new QListViewItem(this, "Current table");
	itemmain->setOpen(TRUE);

	//TODO: real people
	//QListViewItem *tmp2 = new QListViewItem(itemmain, "yoshi");

	addColumn("Players");
	addColumn("Table");
	insertItem(itemmain);

	setRootIsDecorated(TRUE);

	strcpy(userbuf, "");
	m_lock = 0;
	startTimer(300);
}

KGGZ_Users::~KGGZ_Users()
{
}

void KGGZ_Users::add(char *name)
{
	if(strlen(userbuf) > 0) strcat(userbuf, "\n");
	strcat(userbuf, name);
}

void KGGZ_Users::timerEvent(QTimerEvent *e)
{
	char *token;

	if(m_lock) return;
	if(strlen(userbuf) == 0) return;
	m_lock = 1;
	token = strtok(userbuf, "\n");
	while(token != NULL)
	{
		QListViewItem *tmp = new QListViewItem(itemmain, token);
		itemmain->insertItem(tmp);
		printf("Added token %s\n", token);
		token = strtok(NULL, "\n");
		//triggerUpdate();
	}
	strcpy(userbuf, "");
	m_lock = 0;
}