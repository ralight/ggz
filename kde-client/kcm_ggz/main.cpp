#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include "kcm_ggz.h"

int main(int argc, char **argv)
{
	KCMGGZ *kcmggz;

	KCmdLineArgs::init(argc, argv, "kcmggz", "", "");

	KApplication a;
	kcmggz = new KCMGGZ();
	a.setMainWidget(kcmggz);
	kcmggz->show();
	kcmggz->resize(500, 500);
	return a.exec();
}

