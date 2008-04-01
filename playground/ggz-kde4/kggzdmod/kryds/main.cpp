#include "kryds.h"

#include <QCoreApplication>

int main(int argc, char **argv)
{
	QCoreApplication a(argc, argv);

	Kryds *kryds= new Kryds();
	Q_UNUSED(kryds);

	return a.exec();
}

