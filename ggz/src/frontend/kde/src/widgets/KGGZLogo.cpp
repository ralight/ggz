#include "KGGZLogo.h"

#include "KGGZCommon.h"

#include <qpixmap.h>

#include <sys/stat.h>

KGGZLogo::KGGZLogo(QWidget *parent = NULL, char *name = NULL)
: QFrame(parent, name)
{
}

KGGZLogo::~KGGZLogo()
{
}

void KGGZLogo::setLogo(char *logo, char *name)
{
	char buffer[1024];
	struct stat st;

	KGGZDEBUG("Found module icon: %s\n", logo);
	if(!logo)
	{
		strcpy(buffer, KGGZ_DIRECTORY "/images/icons/games/");
		strcat(buffer, name);
		strcat(buffer, ".png");
		logo = buffer;
		KGGZDEBUG("Retrieve Icon from: %s\n", buffer);
		if((stat(buffer, &st) < 0) || (!S_ISREG(st.st_mode))) logo = NULL;
	}
	if(!logo) logo = KGGZ_DIRECTORY "/images/icons/module.png";
	setBackgroundPixmap(QPixmap(logo));
}

void KGGZLogo::mousePressEvent(QMouseEvent *e)
{
	KGGZDEBUG("emit signalInfo()");
	emit signalInfo();
}
