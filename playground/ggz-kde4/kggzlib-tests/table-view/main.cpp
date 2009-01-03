// Table view includes
#include "kggztableview.h"

// KGGZ includes
#include <kggzcore/player.h>

// Qt includes
#include <qapplication.h>

// KDE includes
#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("tableview");

	KGGZCore::Player p1("Some Player", KGGZCore::Player::player);
	KGGZCore::Player p2("Some Bot", KGGZCore::Player::bot);
	KGGZCore::Player p3("Some Reservation", KGGZCore::Player::reserved);
	KGGZCore::Player p4("Abandoned", KGGZCore::Player::abandoned);
	KGGZCore::Player p5("Open", KGGZCore::Player::open);

	QString sp = "Some Spectator";

	TableView dlg;
	dlg.addPlayer(p1);
	dlg.addPlayer(p2);
	dlg.addPlayer(p3);
	dlg.addPlayer(p4);
	dlg.addPlayer(p5);
	dlg.addSpectator(sp);
	return dlg.exec();
}

