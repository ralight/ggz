// Player list includes
#include "roomlist.h"

// Qt includes
#include <qapplication.h>

// Main function, including the about data
int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	RoomList *list = new RoomList();
	Q_UNUSED(list);

	return a.exec();
}

