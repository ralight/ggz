// Player list includes
#include "playerlist.h"

// Qt includes
#include <qapplication.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	PlayerList *list = new PlayerList();
	Q_UNUSED(list);

	return a.exec();
}

