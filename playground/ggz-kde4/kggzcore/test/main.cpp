#include "testkggzcore.h"

#include <QCoreApplication>

int main(int argc, char **argv)
{
	QCoreApplication a(argc, argv);

	QString url;
	if(argc == 2)
		url = argv[1];
	else
		url = "ggz://guest9999@localhost:5688";

	TestKGGZCore *test = new TestKGGZCore(url);
	Q_UNUSED(test);

	return a.exec();
}

