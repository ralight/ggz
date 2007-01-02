#ifndef KGGZGAMES_SYSTEM_H
#define KGGZGAMES_SYSTEM_H

#include <qstring.h>
#include <qwidget.h>

class KGGZSystem
{
	public:
		static void checkInstallation(QWidget *parent, QString appname);
		static void ensureInstallation();
};

#endif

