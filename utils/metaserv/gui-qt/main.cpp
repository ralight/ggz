#include <qapplication.h>
#include "qmetagui.h"

int main(int argc, char **argv)
{
	QApplication a(argc, argv);
	QMetaGUI *metagui;

	metagui = new QMetaGUI();
	a.setMainWidget(metagui);
	metagui->show();
	return a.exec();
}

