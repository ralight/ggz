// Connection dialog includes
#include <kggzlib/tabledialog.h>

// KGGZ includes
#include <kggzcore/gametype.h>

// Qt includes
#include <qapplication.h>

// KDE includes
#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("tabledialog");

	KGGZCore::GameType gametype;

	gametype.setMaxPlayers(10);
	gametype.setMaxBots(5);
	gametype.addNamedBot("Bot 1", "easy");
	gametype.addNamedBot("Bot 2", "hard");
	// FIXME: buddies and grubbies

	/*dlg.addGrubby("Grubby");
	dlg.addBuddy("Buddy 1");
	dlg.addBuddy("Buddy 2");*/

	TableDialog dlg(NULL);
	dlg.setGameType(gametype);
	dlg.setIdentity("josef");
	return dlg.exec();
}

