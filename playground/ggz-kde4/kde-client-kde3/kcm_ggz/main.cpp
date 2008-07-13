#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kaboutdata.h>

#include "kcm_ggz.h"

int main(int argc, char **argv)
{
	KCMGGZ *kcmggz;
	KAboutData about("kcmggz_test",
		"KCMGGZ test program",
		"0.0",
		"Blah",
		KAboutData::License_GPL,
		"Blah",
		"Blah");

	KCmdLineArgs::init(argc, argv, &about);

	KApplication a;
	kcmggz = new KCMGGZ(NULL, NULL, "kcmggz");
	a.setMainWidget(kcmggz);
	kcmggz->show();
	kcmggz->resize(500, 500);
	return a.exec();
}

