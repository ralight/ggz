#include <qapp.h>
#include "gathering.h"

int main(int argc, char **argv)
{
	QApplication *a;
	Gathering *gathering;

	a = new QApplication(argc, argv);
	gathering = new Gathering(NULL, "GGZ Gathering");
	a->setMainWidget(gathering);
	return a->exec();
}
