#ifndef KGAMEHELP_H
#define KGAMEHELP_H

#include <qwidget.h>

class QTextBrowser;

class KGameHelp : public QWidget
{
	Q_OBJECT
	public:
		KGameHelp(QWidget *parent = NULL, const char *name = NULL);
		~KGameHelp();
		void loadHelp(QString name);

	private:
		QTextBrowser *browser;
};

#endif

