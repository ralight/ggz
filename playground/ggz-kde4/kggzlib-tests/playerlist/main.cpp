// Player list includes
#include <kggzlib/playerlist.h>
#include <kggzlib/player.h>
#include <kggzlib/ggzprofile.h>

// Qt includes
#include <qapplication.h>

#include <kcomponentdata.h>
#include <krandom.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("playerlist");

	PlayerList *list = new PlayerList();

	GGZServer ggzserver;
	ggzserver.setCommunity("http://akademy08.ggzcommunity.org/");
	ggzserver.setApi("http://api.ggzcommunity.local/");

	GGZProfile ggzprofile;
	ggzprofile.setUsername("admin");
	ggzprofile.setPassword("admin");
	ggzprofile.setGGZServer(ggzserver);

	list->setGGZProfile(ggzprofile);

	Player *player1 = new Player("someadmin");
	player1->setRole(Player::Admin);
	player1->setStatistics("0-1-5");
	player1->setLag(KRandom::random() % 100);
	list->addPlayer(player1);

	Player *player2 = new Player("somehost");
	player2->setRole(Player::Host);
	player2->setStatistics("12-4-5");
	player2->setLag(KRandom::random() % 100);
	list->addPlayer(player2);

	Player *player3 = new Player("someregular");
	player3->setRole(Player::Registered);
	player3->setStatistics("0-0-0");
	player3->setLag(KRandom::random() % 100);
	list->addPlayer(player3);

	Player *player4 = new Player("someguest");
	player4->setRole(Player::Guest);
	player4->setStatistics("0-0-0");
	player4->setLag(KRandom::random() % 100);
	list->addPlayer(player4);

	Player *player5 = new Player("somebot");
	player5->setRole(Player::Bot);
	player5->setStatistics("0-0-0");
	player5->setLag(KRandom::random() % 100);
	list->addPlayer(player5);

	// Note: role and relation are mostly orthogonal
	// We assume buddies and banned players to be regularly registered

	Player *player6 = new Player("ignoredplayer");
	player6->setRole(Player::Registered);
	player6->setRelation(Player::Ignored);
	player6->setStatistics("0-0-0");
	player6->setLag(KRandom::random() % 100);
	list->addPlayer(player6);

	Player *player7 = new Player("buddyplayer");
	player7->setRole(Player::Registered);
	player7->setRelation(Player::Buddy);
	player7->setStatistics("0-0-0");
	player7->setLag(KRandom::random() % 100);
	list->addPlayer(player7);

	list->show();

	return a.exec();
}

