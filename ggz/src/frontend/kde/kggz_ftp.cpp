#include "kggz_ftp.h"

#include <klocale.h>

KGGZ_Ftp::KGGZ_Ftp(QWidget *parent, char *name)
: KGGZ_Profiles(parent, name)
{
	addField("Description", NULL, NULL);
	addField("Host", "Host", NULL);
	addField("Path", "Path", "/pub/ggz");
	setSectionName("FTP-Servers");
	setCaption(i18n("FTP-Servers"));
	readEntries();
}

KGGZ_Ftp::~KGGZ_Ftp()
{
}
