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

	KCmdLineOptions options;
	options.add("+[url]", ki18n("URL of the GGZ Gaming Zone server"));

	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication app;
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

	QString url;
	if(args->count() == 1)
		url = args->arg(0);

	args->clear();

	Vencedor *vencedor = new Vencedor(url);
	Q_UNUSED(vencedor);

	return app.exec();
}

