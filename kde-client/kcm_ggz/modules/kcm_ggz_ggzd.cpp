#include "kcm_ggz_ggzd.h"
#include "kcm_ggz_ggzd.moc"

#include <klistview.h>
#include <qlayout.h>

KCMGGZGgzd::KCMGGZGgzd(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	QVBoxLayout *vbox;

	view = new KListView(this);
	view->addColumn("Parameters");
	view->addColumn("Values");
	view->setRootIsDecorated(true);

	add("General", "Server");
	add("General", "AdminName");
	add("General", "AdminEmail");
	add("General", "Port");
	add("General", "EncryptionUse");
	add("General", "EncryptionPassword");
	add("General", "EncryptionCert");
	add("General", "EncryptionKey");

	add("Logs", "LogFile");
	add("Logs", "LogTypes");
	add("Logs", "DebugFile");
	add("Logs", "DebugTypes");
	add("Logs", "Facility");
	add("Logs", "PIDInLogs");
	add("Logs", "TimeInLogs");
	add("Logs", "GameTypeInLogs");

	add("Directories", "ConfDir");
	add("Directories", "DataDir");
	add("Directories", "GameDir");

	add("Files", "MOTD");

	add("Games", "GameList");
	add("Games", "IgnoredGames");
	add("Games", "RoomList");
	add("Games", "IgnoredRooms");

	add("Miscellaneous", "PingFrequency");
	add("Miscellaneous", "LagClass1");
	add("Miscellaneous", "LagClass2");
	add("Miscellaneous", "LagClass3");
	add("Miscellaneous", "LagClass4");
	add("Miscellaneous", "HostnameLookup");

	vbox = new QVBoxLayout(this, 5);
	vbox->add(view);

	load();
}

KCMGGZGgzd::~KCMGGZGgzd()
{
}

void KCMGGZGgzd::load()
{
}

void KCMGGZGgzd::save()
{
}

const char *KCMGGZGgzd::caption()
{
	return "Configuration of ggzd";
}

extern "C"
{
	KCMGGZPane *kcmggz_init(QWidget *parent, const char *name)
	{
		return new KCMGGZGgzd(parent, name);
	}
}

void KCMGGZGgzd::add(QString category, QString key)
{
	QListViewItem *cat, *item;
	QString value;

	cat = NULL;
	for(QListViewItem *i = view->firstChild(); i; i = i->nextSibling())
	{
		if(i->text(0) == category)
		{
			cat = i;
			break;
		}
	}
	if(!cat) cat = new QListViewItem(view, category);

	value = "0x00";

	item = new QListViewItem(cat, key, value);
}

