//////////////////////////////////////////////////////////////////////
// KTicTacTux
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
// Published under GNU GPL conditions
//////////////////////////////////////////////////////////////////////

// KTicTacTux includes
#include "ktictactuxwin.h"

// KDE includes
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

// Available command line options
static const KCmdLineOptions op[] = {
	{"ggz", I18N_NOOP("Request GGZ game explicitely"), 0},
	{0, 0, 0}};

// Main function, including the about data
int main(int argc, char **argv)
{
	KAboutData *aboutData;
	KTicTacTuxWin *ktictactuxwin;
	KCmdLineArgs *args;

	aboutData = new KAboutData("ktictactux",
		I18N_NOOP("KTicTacTux"),
		"0.0.6",
		I18N_NOOP("This is a TicTacToe game for KDE."),
		KAboutData::License_GPL,
		"Copyright (C) 2001, 2002 Josef Spillner",
		I18N_NOOP("This game is part of the GGZ Gaming Zone."),
		"http://ggz.sourceforge.net/games/ktictactux/",
		"dr_maux@users.sourceforge.net");
	aboutData->addAuthor("Josef Spillner", I18N_NOOP("Inventor"), "dr_maux@users.sourceforge.net", "http://mindx.sourceforge.net");
	aboutData->setTranslator(I18N_NOOP("TRANSLATOR-NAME"), I18N_NOOP("TRANSLATOR-EMAIL"));

	KCmdLineArgs::init(argc, argv, aboutData);
	KCmdLineArgs::addCmdLineOptions(op);
	args = KCmdLineArgs::parsedArgs();

	KApplication a;
	ktictactuxwin = new KTicTacTuxWin(NULL, "KTicTacTuxWin");
	if(args->isSet("ggz"))
	{
		ktictactuxwin->enableNetwork(true);
		ktictactuxwin->tux()->setOpponent(PLAYER_NETWORK);
	}
	else ktictactuxwin->tux()->setOpponent(PLAYER_AI);
	ktictactuxwin->tux()->init();

	a.setMainWidget(ktictactuxwin);
	return a.exec();
}

