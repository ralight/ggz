#include "kggz_users.h"
#include <string.h>
#include <stdio.h>
#include <klocale.h>

QListViewItem *itemmain;
char userbuf[4096];
char userbuf2[4096];
int m_lock;

KGGZ_Users::KGGZ_Users(QWidget *parent, char *name)
: QListView(parent, name)
{

	itemmain = new QListViewItem(this, "Current table");
	itemmain->setOpen(TRUE);

	//TODO: real people
	//QListViewItem *tmp2 = new QListViewItem(itemmain, "yoshi");

	addColumn(i18n("Players"));
	addColumn(i18n("Table"));
	insertItem(itemmain);

	setRootIsDecorated(TRUE);

	strcpy(userbuf, "");
	strcpy(userbuf2, "");
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

void KGGZ_Users::remove(char *name)
{
	if(strlen(userbuf2) > 0) strcat(userbuf2, "\n");
	strcat(userbuf2, name);
}

void KGGZ_Users::timerEvent(QTimerEvent *e)
{
	char *token;
	const char *tokentmp;

	if(m_lock) return;
	if(strlen(userbuf) != 0)
	{
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

	if(strlen(userbuf2) != 0)
	{
		m_lock = 1;

		token = strtok(userbuf2, "\n");
		while(token != NULL)
		{
			printf("It must be %s!\n", token);
			QListViewItem *tmp = firstChild()->firstChild();
			while(tmp)
			{
				tokentmp = tmp->text(0);
				printf("Is it %s?\n", tokentmp);
				if(strcmp(tokentmp, token) == 0)
				{
					printf("Yes it is!\n");
					itemmain->takeItem(tmp);
					tmp = NULL;
				}
				else
				{
					printf("Nope. Next please!\n");
					//tmp = tmp->itemBelow(); won't work
					//tmp = tmp->nextSibling(); won't work
					//tmp = tmp->itemAbove(); won't work
					if(tmp->itemBelow() == NULL) printf("itemBelow is NULL!\n");
					if(tmp->itemAbove() == NULL) printf("itemAbove is NULL!\n");
					if(tmp->nextSibling() == NULL) printf("nextSibling is NULL!\n");
					tmp = tmp->itemAbove();
				}
			}
			printf("Removed token %s\n", token);
			token = strtok(NULL, "\n");
		}
		strcpy(userbuf2, "");

		m_lock = 0;
	}
}
