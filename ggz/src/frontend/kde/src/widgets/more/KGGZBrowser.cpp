#include "KGGZBrowser.h"

#include <qlayout.h>
#include <khtmlview.h>
#include "KGGZCommon.h"

KGGZBrowser::KGGZBrowser(QWidget *parent = NULL, char *name = NULL)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	KHTMLView *view;

	setBackgroundColor(QColor(255.0, 0.0, 255.0));

	m_part = new KHTMLPart(this);
	//m_part->view()->resize(500, 400);
	//part->enableJava(ggzcore_conf_read_int("KGGZ-Settings", "Java", 0));
	m_part->openURL(KGGZ_DIRECTORY "/help/index.html");

	m_edit = new QLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(m_part->view());
	vbox->add(m_edit);

	connect(m_part->browserExtension(), SIGNAL(openURLRequest(const KURL&, const KParts::URLArgs&)),
		SLOT(slotRequest(const KURL&, const KParts::URLArgs&)));
}

KGGZBrowser::~KGGZBrowser()
{
}

void KGGZBrowser::slotRequest(const KURL& url, const KParts::URLArgs& args)
{
	m_part->openURL(url.url());
}
