#include <kapplication.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "mainwindow.h"

static const KCmdLineOptions op[] =
{
	{"ggz", I18N_NOOP("Play in GGZ mode"), 0},
	{0, 0, 0}
};

int main(int argc, char **argv)
{
	MainWindow *mainwindow;
	KCmdLineArgs *args;
	KAboutData data("fyrdman",
		I18N_NOOP("Fyrdman"),
		"0.1",
		I18N_NOOP("KDE client for the Hastings1066 game."),
		KAboutData::License_GPL,
		I18N_NOOP("Copyright (C) 2002, 2003 Josef Spillner"),
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://ggz.sourceforge.net/games/fyrdman/",
		"dr_maux@users.sourceforge.net");

	data.addAuthor("Josef Spillner", I18N_NOOP("Initial developer"), "josef@ggzgamingzone.org", "http://mindx.dyndns.org");
	data.setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, &data);
	KCmdLineArgs::addCmdLineOptions(op);
	args = KCmdLineArgs::parsedArgs();

	KApplication app;

	mainwindow = new MainWindow();
	app.setMainWidget(mainwindow);
	if(args->isSet("ggz"))
	{
		mainwindow->enableNetwork();
	}
	return app.exec();
}

