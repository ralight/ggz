#include <kaboutdata.h>
#include <kapp.h>
#include <kcmdlineargs.h>

#include "toplevel.h"

int main(int argc, char **argv)
{
	KAboutData aboutData("koenig",
						"Koenig",
						"0.01",
						"GGZ Chess Frontend for KDE",
						KAboutData::License_GPL_V2,
						"Copyright (C) 2001 Tobias Koenig & Josef Spillner",
						NULL,
						"http://www.geocities.com/linuxcode/kchess",
						"dr_maux@users.sourceforge.net");

	aboutData.addAuthor("Tobias Koenig", "Creator", "tokoe82@yahoo.de", "http://www.geocities.com/linuxcode");
	aboutData.addAuthor("Josef Spillner", "Maintainer", "dr_maux@users.sourceforge.net", "http://mindx.sourceforge.net");

	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication *app = new KApplication;
	TopLevel *toplevel = new TopLevel;
	CHECK_PTR(toplevel);
	toplevel->show();
	app->setMainWidget(toplevel);
	int retval = app->exec();
	delete app;
	return retval;	
}

