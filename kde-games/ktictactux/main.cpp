#include <kapp.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include "ktictactux.h"

#include <stdio.h>

static const KCmdLineOptions op[] = {
	{"o", I18N_NOOP("Launch game instead of joining"), 0},
	{"ggz", I18N_NOOP("Request GGZ game explicitely"), 0},
	{0, 0, 0}};

int main(int argc, char **argv)
{
	KApplication *a;
	KAboutData *aboutData;
	KTicTacTux *ktictactux;
	KCmdLineArgs *args;

	aboutData = new KAboutData("KTicTacTux",
		I18N_NOOP("TicTacToe with animal images"),
		"0.0.1",
		I18N_NOOP("test1"),
		KAboutData::License_GPL,
		"(C) 2001 Josef Spillner",
		I18N_NOOP("You can update this game with KGGZ!"),
		"http://mindx.sourceforge.net",
		"dr_maux@users.sourceforge.net");
	aboutData->addAuthor("Josef Spillner", I18N_NOOP("this is a test"), "dr_maux@users.sourceforge.net", "http://mindx.sourceforge.net");

	KCmdLineArgs::init(argc, argv, aboutData);
	KCmdLineArgs::addCmdLineOptions(op);
	args = KCmdLineArgs::parsedArgs();

	a = new KApplication();
	ktictactux = new KTicTacTux(NULL, "KTicTacTux");
	if(args->isSet("ggz")) ktictactux->setOpponent(PLAYER_NETWORK);
printf("initialize\n");
	ktictactux->init();
printf("done; execute now\n");
	a->setMainWidget(ktictactux);
	return a->exec();
}
