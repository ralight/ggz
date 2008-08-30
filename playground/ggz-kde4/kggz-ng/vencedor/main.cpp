#include "vencedor.h"

#include <QTextCodec>

#include <kaboutdata.h>
#include <kapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>

int main(int argc, char **argv)
{
	KAboutData about("vencedor",
		0,
		ki18n("Vencedor"),
		"0.99.4+",
		ki18n("Core client for the GGZ Gaming Zone"),
		KAboutData::License_GPL,
		ki18n("Copyright (C) 2008 Josef Spillner"),
		ki18n("This application is part of the GGZ Gaming Zone."),
		"http://www.ggzgamingzone.org/clients/vencedor/",
		"josef@ggzgamingzone.org");
	about.addAuthor(ki18n("Josef Spillner"), ki18n("Maintainer"), "josef@ggzgamingzone.org");

	KCmdLineArgs::init(argc, argv, &about);

	KApplication app;

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

	QString url;
	if(argc == 2)
		url = argv[1];
	else
		url = "ggz://guest9999@localhost:5688";

	Vencedor *vencedor = new Vencedor(url);
	Q_UNUSED(vencedor);

	return app.exec();
}

