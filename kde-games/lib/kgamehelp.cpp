#include "kgamehelp.h"

#include <klocale.h>

#include <qlayout.h>
#include <qtextbrowser.h>
#include <qpushbutton.h>

KGameHelp::KGameHelp(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QPushButton *ok;

	browser = new QTextBrowser(this);

	ok = new QPushButton(i18n("Dismiss"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(browser);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(close()));

	setCaption(i18n("Game help"));
	resize(600, 500);
}

KGameHelp::~KGameHelp()
{
}

void KGameHelp::loadHelp(QString name)
{
	browser->setSource(name);
}

