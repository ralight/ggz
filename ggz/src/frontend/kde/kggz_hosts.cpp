#include "kggz_hosts.h"

#include <klocale.h>

KGGZ_Hosts::KGGZ_Hosts(QWidget *parent, char *name)
: KGGZ_Profiles(parent, name)
{
	addField("Description", NULL, NULL);
	addField("Server host", "Host", NULL);
	addField("Port", "Port", "5688");
	addField("Login", "Login", "");
	addField("Password", "Password", "");
	setSectionName("Servers");
	setCaption(i18n("Server Profiles"));
	readEntries();
}

KGGZ_Hosts::~KGGZ_Hosts()
{
}
