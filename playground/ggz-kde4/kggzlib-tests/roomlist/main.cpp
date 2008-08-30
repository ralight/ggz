// Player list includes
#include <kggzlib/roomlist.h>
#include <kggzlib/room.h>

// Qt includes
#include <qapplication.h>

#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("roomlist");

	RoomList *list = new RoomList();

	Room *room1 = new Room("Tic-Tac-Toe");
	room1->setLogo("tictactoe.png");
	room1->setDescription("A room to do fun things in...");
	room1->setPlayers(99);
	room1->setFavourite(99);
	list->addRoom(room1);

	Room *room2 = new Room("Chess");
	room2->setLogo("chess.png");
	room2->setModule(true);
	room2->setPlayers(42);
	list->addRoom(room2);

	Room *room3 = new Room("Connect The Dots");
	room3->setLogo("dots.png");
	room3->setDescription("Elite players only");
	room3->setAccess(Room::Locked);
	list->addRoom(room3);

	list->show();

	return a.exec();
}

