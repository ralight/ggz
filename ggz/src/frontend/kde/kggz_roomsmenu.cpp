#include "kggz_roomsmenu.h"
#include "kggz.h"
#include <stdio.h>

KGGZ_RoomsMenu::KGGZ_RoomsMenu(char *gamename, QWidget *parent, char *name)
: KPopupMenu(parent, name)
{
	/*
	if(strcmp(gamename, "TicTacToe") == 0)
	{
		insertItem("Classic");
		insertItem("Room of Doom");
	}
	if(strcmp(gamename, "Hastings1066") == 0)
	{
		insertItem("Example game");
	}
	*/
}

KGGZ_RoomsMenu::~KGGZ_RoomsMenu()
{
}

void KGGZ_RoomsMenu::add(char *name)
{
	static int x = 0;
	KPopupMenu *tmpmenu;

//new KGGZ_RoomsMenu(name, /*this???*/NULL, NULL)
	KGGZ::menuRooms()->setEnabled(TRUE);
	//tmpmenu = new KPopupMenu(/*name*/NULL, KGGZ::menuRooms(), NULL);
	//tmpmenu = new KPopupMenu(NULL, NULL);
	//tmpmenu->insertItem(QString("Play ") + QString(name));
	//KGGZ::menuRooms()->insertItem(name, tmpmenu, KGGZ::MENU_ROOMS_SLOTS + x);
	KGGZ::menuRooms()->insertItem(name, KGGZ::MENU_ROOMS_SLOTS + x);
	x++;
	//connect(tmpmenu, SIGNAL(activated(int)), this, SLOT(handleMenu(int)));
	//KGGZ::menuRooms()->insertItem(name, new KPopupMenu("sample game 2", KGGZ::menuRooms(), NULL), KGGZ::MENU_ROOMS_SLOTS);
	// unclean!
	KGGZ::menuClient()->setItemEnabled(KGGZ::MENU_CLIENT_CHAT, TRUE);
	KGGZ::menuClient()->setItemEnabled(KGGZ::MENU_CLIENT_TABLES, TRUE);
	KGGZ::menuClient()->setItemEnabled(KGGZ::MENU_CLIENT_PLAYERS, TRUE);
	KGGZ::menuGGZ()->setItemEnabled(KGGZ::MENU_GGZ_CONNECT, FALSE);
	KGGZ::menuGGZ()->setItemEnabled(KGGZ::MENU_GGZ_DISCONNECT, TRUE);
	KGGZ::menuGame()->setEnabled(TRUE);
}

void KGGZ_RoomsMenu::removeAll()
{
	KGGZ::menuRooms()->setEnabled(FALSE);
	//unclean
	KGGZ::menuClient()->setItemEnabled(KGGZ::MENU_CLIENT_CHAT, FALSE);
	KGGZ::menuClient()->setItemEnabled(KGGZ::MENU_CLIENT_TABLES, FALSE);
	KGGZ::menuClient()->setItemEnabled(KGGZ::MENU_CLIENT_PLAYERS, FALSE);
	KGGZ::menuGGZ()->setItemEnabled(KGGZ::MENU_GGZ_CONNECT, TRUE);
	KGGZ::menuGGZ()->setItemEnabled(KGGZ::MENU_GGZ_DISCONNECT, FALSE);
	KGGZ::menuGame()->setEnabled(FALSE);
}

void KGGZ_RoomsMenu::handleMenu(int id)
{
	printf("%i\n", id);
}