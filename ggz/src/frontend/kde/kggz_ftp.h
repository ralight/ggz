#ifndef KGGZ_FTP_H
#define KGGZ_FTP_H

#include "kggz_profiles.h"

class KGGZ_Ftp : public KGGZ_Profiles
{
Q_OBJECT
public:
	KGGZ_Ftp(QWidget *parent, char *name);
	~KGGZ_Ftp();
};

#endif
