#include "testkggzcore.h"

#include <QCoreApplication>

int main(int argc, char **argv)
{
	QCoreApplication a(argc, argv);

	TestKGGZCore *test = new TestKGGZCore();
	Q_UNUSED(test);

	return a.exec();
}

