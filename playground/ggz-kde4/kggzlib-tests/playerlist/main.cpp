// Player list includes
#include <kggzlib/playerlist.h>
#include <kggzlib/player.h>

// Qt includes
#include <qapplication.h>

#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("playerlist");

	PlayerList *list = new PlayerList();

	Player *player1 = new Player("someadmin");
	player1->setRole(Player::Admin);
	player1->setStatistics("0-1-5");
	player1->setLag(25);
	player1->setRelation(Player::Buddy);
	list->addPlayer(player1);

	Player *player2 = new Player("somehost");
	player2->setRole(Player::Host);
	player2->setStatistics("12-4-5");
	player2->setLag(69);
	list->addPlayer(player2);

	list->show();

	return a.exec();
}

