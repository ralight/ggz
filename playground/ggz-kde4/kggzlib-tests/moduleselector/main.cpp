// Player list includes
#include <kggzlib/moduleselector.h>
#include <kggzcore/module.h>

// Qt includes
#include <qapplication.h>

#include <kcomponentdata.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	KComponentData data("moduleselector");

	ModuleSelector *selector = new ModuleSelector(NULL);
	selector->setGameType("TicTacToe", "dio/5+dev");

	selector->show();

	return a.exec();
}

