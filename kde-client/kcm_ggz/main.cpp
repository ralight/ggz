#include <kapp.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include "kcm_ggz.h"

int main(int argc, char **argv)
{
	KApplication *a;
	KCMGGZ *kcmggz;

	KCmdLineArgs::init(argc, argv, "kcmggz", "", "");

	a = new KApplication();
	kcmggz = new KCMGGZ();
	a->setMainWidget(kcmggz);
	kcmggz->show();
	kcmggz->resize(500, 500);
	a->exec();
	return 0;
}

