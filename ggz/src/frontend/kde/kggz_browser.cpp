#include "kggz_browser.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef USE_KHTML
#include <khtmlview.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <ggzcore.h>
#endif

#include <stdio.h>
#include <kmessagebox.h>
#include <klocale.h>

KGGZ_Browser::KGGZ_Browser(QWidget *parent, char *name)
#ifdef USE_KHTML
: QWidget(parent, name)
#else
: QTextBrowser(parent, name)
#endif
{
#ifdef USE_KHTML
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	KHTMLView *view;
	QLabel *label;
	QPushButton *button_ggz, *button_mindx;

	part = new KHTMLPart(this);
	//part = new KHTMLPart(view, NULL, "name");
	//view = new KHTMLView(part, this, "khtml");
	//view->resize(500, 400);
	part->view()->resize(500, 400); // invent a clever resize policy!

	part->enableJava(ggzcore_conf_read_int("KGGZ-Settings", "Java", 0));

#ifdef HAVE_CONFIG_H
	part->openURL(QString(instdir) + "help/index.html");
#endif

	label = new QLabel("URL:", this);
	ed = new QLineEdit(this);
	button_ggz = new QPushButton("GGZ Home", this);
	button_mindx = new QPushButton("MindX Home", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(part->view());

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(label);
	hbox->add(ed);
	hbox->add(button_ggz);
	hbox->add(button_mindx);

	connect(part->browserExtension(), SIGNAL(openURLRequest(const KURL &, const KParts::URLArgs &)), SLOT(slotRequest(const KURL &, const KParts::URLArgs &)));
	connect(ed, SIGNAL(returnPressed()), SLOT(slotExplicitRequest()));
	connect(button_ggz, SIGNAL(clicked()), SLOT(slotDirectRequestGGZ()));
	connect(button_mindx, SIGNAL(clicked()), SLOT(slotDirectRequestMindX()));
#else
#ifdef HAVE_CONFIG_H
	setSource(instdir + "help/index.html");
#endif
#endif
}

KGGZ_Browser::~KGGZ_Browser()
{
}

void KGGZ_Browser::slotExplicitRequest()
{
	if(strncmp(ed->text(), "ggz", 3) == 0)
		KMessageBox::information(this, i18n("The ggz protocol is not implemented yet."), i18n("Warning!"));
	else
		part->openURL(ed->text());
}

void KGGZ_Browser::slotRequest(const KURL &url, const KParts::URLArgs &)
{
	printf("slotrequest\n");
	ed->setText(url.url());
	slotExplicitRequest();
}

void KGGZ_Browser::slotDirectRequestGGZ()
{
	ed->setText("http://ggz.sourceforge.net");
	slotExplicitRequest();
}

void KGGZ_Browser::slotDirectRequestMindX()
{
	ed->setText("http://mindx.sourceforge.net");
	slotExplicitRequest();
}