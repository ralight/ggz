#include "kcm_ggz_ggzd.h"
#include "kcm_ggz_ggzd.moc"

#include <klistview.h>
#include <klocale.h>
#include <ksimpleconfig.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "config.h"

KCMGGZGgzd::KCMGGZGgzd(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *label;
	QPushButton *button;

	m_firsttime = 1;

	view = new KListView(this);
	view->addColumn(i18n("Parameters"));
	view->addColumn(i18n("Default"));
	view->addColumn(i18n("Values"));
	view->setRootIsDecorated(true);

	add("General", "Server", "Honest Harry's Server");
	add("General", "AdminName", "Honest Harry");
	add("General", "AdminEmail", "");
	add("General", "Port", "5688");
	add("General", "EncryptionUse", "0");
	add("General", "EncryptionPassword", "");
	add("General", "EncryptionCert", "");
	add("General", "EncryptionKey", "");
	add("General", "DatabaseHost", "localhost");
	add("General", "DatabaseName", "ggz");
	add("General", "DatabaseUsername", "ggzd");
	add("General", "DatabasePassword", "ggzd");

	add("Logs", "LogFile", "/var/log/ggzd/ggzd.log");
	add("Logs", "LogTypes", "All");
	add("Logs", "DebugFile", "/var/log/ggzd/ggzd.debug");
	add("Logs", "DebugTypes", "All");
	add("Logs", "Facility", "local0");
	add("Logs", "PIDInLogs", "0");
	add("Logs", "TimeInLogs", "1");
	add("Logs", "GameTypeInLogs", "0");

	add("Directories", "ConfDir", "/usr/local/etc/ggzd");
	add("Directories", "DataDir", "/usr/local/var/ggzd");
	add("Directories", "GameDir", "/usr/local/lib/ggzd");

	add("Files", "MOTD", "ggzd.motd");

	add("Games", "GameList", "");
	add("Games", "IgnoredGames", "");
	add("Games", "RoomList", "");
	add("Games", "IgnoredRooms", "");

	add("Miscellaneous", "PingFrequency", "10");
	add("Miscellaneous", "LagClass1", "500");
	add("Miscellaneous", "LagClass2", "1000");
	add("Miscellaneous", "LagClass3", "2000");
	add("Miscellaneous", "LagClass4", "5000");
	add("Miscellaneous", "HostnameLookup", "1");

	label = new QLabel(i18n("Location of the ggzd configuration file"), this);
	locator = new QLineEdit("", this);
	button = new QPushButton(i18n("Update"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(view);
	vbox->add(label);
	hbox = new QHBoxLayout(vbox);
	hbox->add(locator);
	hbox->add(button);

	connect(button, SIGNAL(clicked()), SLOT(reload()));

	reload();
}

KCMGGZGgzd::~KCMGGZGgzd()
{
}

void KCMGGZGgzd::load()
{
	KSimpleConfig conf("kcmggz_ggzd");
	conf.setGroup("General");
	locator->setText(conf.readEntry("Configuration"));

	if(locator->text().isEmpty())
		locator->setText(PREFIX "/etc/ggzd/ggzd.conf");
}

void KCMGGZGgzd::save()
{
	KSimpleConfig conf("kcmggz_ggzd");
	conf.setGroup("General");
	conf.writeEntry("Configuration", locator->text());
}

QString KCMGGZGgzd::caption()
{
	return i18n("Configuration of ggzd");
}

extern "C"
{
	KCMGGZPane *kcmggz_init(QWidget *parent, const char *name)
	{
		return new KCMGGZGgzd(parent, name);
	}
}

void KCMGGZGgzd::add(QString category, QString key, QString def)
{
	KListViewItem *cat, *item;
	QString value;

	cat = NULL;
	for(QListViewItem *i = view->firstChild(); i; i = i->nextSibling())
	{
		if(i->text(0) == category)
		{
			cat = reinterpret_cast<KListViewItem*>(i);
			break;
		}
	}
	if(!cat)
	{
		cat = new KListViewItem(view, category);
		cat->setOpen(true);
	}

	value = "0x00";

	item = new KListViewItem(cat, key, value, def);
}

void KCMGGZGgzd::reload()
{
	QString entry;
	KSimpleConfig conf(locator->text());

	conf.reparseConfiguration();
	if(!m_firsttime)
	{
		switch(conf.getConfigState())
		{
			case KSimpleConfig::NoAccess:
				/* FIXME: bug in KConfigBase or KSimpleConfig in KDE 2.2.2? */
				//KMessageBox::error(this,
				//	i18n("The ggzd configuration file could not be found\n"
				//	"at the given location (%1).").arg(locator->text()),
				//	i18n("Read error"));
				//return;
				break;
			case KSimpleConfig::ReadOnly:
				KMessageBox::information(this,
					i18n("You don't have the permission to modify any entries.\n"
					"All information is read-only."),
					i18n("Write error"));
				break;
			case KSimpleConfig::ReadWrite:
				// ok
				break;
		}

		emit signalChanged();
	}

	for(QListViewItem *i = view->firstChild(); i; i = i->nextSibling())
	{
		conf.setGroup(i->text(0));
		for(QListViewItem *j = i->firstChild(); j; j = j->nextSibling())
		{
			entry = conf.readEntry(j->text(0));
			if(entry == QString::null)
			{
				entry = QString("(default: %1)").arg(j->text(2));
			}
			j->setText(1, entry);
		}
	}

	m_firsttime = 0;
}

