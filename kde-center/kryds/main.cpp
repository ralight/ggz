#include "kryds.h"

#include <QCoreApplication>

#include <kcomponentdata.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kdebug.h>

int main(int argc, char **argv)
{
	KAboutData about("kryds",
		QByteArray(),
		ki18n("Kryds"),
		"0.1",
		ki18n("TTT game server based on kggzdmod"),
		KAboutData::License_GPL_V2,
		ki18n("Copyright (C) 2008 Josef Spillner"),
		KLocalizedString(),
		"http://www.ggzgamingzone.org/gameservers/kryds/"
	);
	KComponentData cd(about);

	QCoreApplication a(argc, argv);

	if(!KGGZdMod::Module::isGGZ())
	{
		qFatal("Error: Game server only runs in GGZ mode.");
		return 1;
	}

	qDebug("test debug: qDebug()");
	kDebug() << "test debug: kDebug()";

	Kryds *kryds = new Kryds();
	Q_UNUSED(kryds);

	return a.exec();
}

