#ifndef KGGZ_HOST_H
#define KGGZ_HOSTS_H

#include "kggz_profiles.h"

class KGGZ_Hosts : public KGGZ_Profiles
{
Q_OBJECT
public:
	KGGZ_Hosts(QWidget *parent, char *name);
	~KGGZ_Hosts();
};

#endif