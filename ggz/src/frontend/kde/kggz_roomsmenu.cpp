#include "kggz_roomsmenu.h"

KGGZ_RoomsMenu::KGGZ_RoomsMenu(char *gamename, QWidget *parent, char *name)
: QPopupMenu(parent, name)
{
	if(strcmp(gamename, "TicTacToe") == 0)
	{
		insertItem("Classic");
		insertItem("Room of Doom");
	}
	if(strcmp(gamename, "Hastings1066") == 0)
	{
		insertItem("Example game");
	}
}

KGGZ_RoomsMenu::~KGGZ_RoomsMenu()
{
}
