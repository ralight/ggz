// Connection dialog includes
#include <kggzlib/tabledialog.h>

// Qt includes
#include <qapplication.h>

// KDE includes
#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("tabledialog");

	TableDialog dlg(NULL);
	/*dlg.initLauncher("josef", 10, 5);
	dlg.addGrubby("Grubby");
	dlg.addBot("Bot 1", "easy");
	dlg.addBot("Bot 2", "hard");
	dlg.addBuddy("Buddy 1");
	dlg.addBuddy("Buddy 2");*/
	return dlg.exec();
}

