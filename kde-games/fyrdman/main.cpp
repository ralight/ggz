#include <kapplication.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "mainwindow.h"

int main(int argc, char **argv)
{
	MainWindow *mainwindow;
	KAboutData data("fyrdman",
		I18N_NOOP("Fyrdman"),
		"0.1",
		I18N_NOOP("KDE client for the Hastings1066 game."),
		KAboutData::License_GPL,
		QString::null,
		"dr_maux@users.sourceforge.net");

	KCmdLineArgs::init(argc, argv, &data);

	KApplication app;

	mainwindow = new MainWindow();
	app.setMainWidget(mainwindow);
	return app.exec();
}

