#include "ggzap_config.h"

#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <stdlib.h>

#include "GGZCoreConfio.h"

GGZapConfig::GGZapConfig(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox;
	QLabel *labelserver, *labelname;
	QPushButton *ok;

	labelserver = new QLabel(i18n("GGZap Reservation server"), this);
	labelname = new QLabel(i18n("Player name (12 chars maximum)"), this);

	ok = new QPushButton("OK", this);

	m_server = new QLineEdit(this);
	m_username = new QLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(labelserver);
	vbox->add(m_server);
	vbox->add(labelname);
	vbox->add(m_username);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccept()));

	configfile = new QString(getenv("HOME"));
	configfile->append("/.ggz/ggzap.rc");
	configLoad();

	setFixedSize(200, 150);
	setCaption(i18n("GGZap Configuration"));
	show();
}

GGZapConfig::~GGZapConfig()
{
	delete configfile;
}

void GGZapConfig::slotAccept()
{
	configSave();
	close();
}

void GGZapConfig::configLoad()
{
	GGZCoreConfio *conf;

	conf = new GGZCoreConfio(configfile->latin1(), GGZCoreConfio::readonly);
	m_server->setText(conf->read("Global", "Server", "141.30.227.122"));
	m_username->setText(conf->read("Global", "Username", ""));
	delete conf;
}

void GGZapConfig::configSave()
{
	GGZCoreConfio *conf;

	if(m_username->text().length() > 12) m_username->setText(m_username->text().left(12));
	conf = new GGZCoreConfio(configfile->latin1(), GGZCoreConfio::readwrite | GGZCoreConfio::create);
	conf->write("Global", "Server", m_server->text().latin1());
	conf->write("Global", "Username", m_username->text().latin1());
	conf->commit();
	delete conf;
}

