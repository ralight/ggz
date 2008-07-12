#include <kapplication.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include "toplevel.h"

int main(int argc, char **argv)
{
	Toplevel *top;
	KAboutData *about;

	about = new KAboutData("kgrubby",
		I18N_NOOP("KGrubby"),
		"0.1",
		I18N_NOOP("Chatbot configuration"),
		KAboutData::License_GPL,
		"Copyright (C) 2005 Josef Spillner",
		I18N_NOOP("Configuration of grubby, the GGZ chatbot"),
		"http://www.ggzgamingzone.org/utils/kgrubby/",
		"josef@ggzgamingzone.org");
	about->addAuthor("Josef Spillner", I18N_NOOP("Developer"), "josef@ggzgamingzone.org");
	about->setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, about);

	KApplication a;
	top = new Toplevel();
	a.setMainWidget(top);
	return a.exec();
}

