#include <kdebug.h>
#include <kgenericfactory.h>
#include <kconfig.h>

#include <qlineedit.h>
#include <qlayout.h>
#include <qlabel.h>

#include "plugin_kcm.h"

K_EXPORT_COMPONENT_FACTORY(kcm_kopete_ggz, KGenericFactory<GGZPluginPreferences>("kcm_kopete_ggz"))

GGZPluginPreferences::GGZPluginPreferences(QWidget *parent, const char *name, const QStringList& args)
: KCModule(parent, name, args)
{
	QVBoxLayout *vbox;
	QLabel *serverdesc;

	serverdesc = new QLabel(i18n("GGZ server"), this);
	m_server = new QLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(serverdesc);
	vbox->add(m_server);
	vbox->addStretch(1);

	connect(m_server, SIGNAL(textChanged(const QString&)), SLOT(changed()));
}

void GGZPluginPreferences::load()
{
	KConfig conf("kopeteggzrc");
	conf.setGroup("default");

	QString server = conf.readEntry("Server", "live.ggzgamingzone.org");

	m_server->setText(server);
}

void GGZPluginPreferences::save()
{
	KConfig conf("kopeteggzrc");
	conf.setGroup("default");

	QString server = m_server->text();

	conf.writeEntry("Server", server);

	conf.sync();

	changed(false);
}

#include "plugin_kcm.moc"

// vim: set noet ts=4 sts=4 sw=4:

