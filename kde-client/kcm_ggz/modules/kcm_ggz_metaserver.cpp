#include "kcm_ggz_metaserver.h"
#include "kcm_ggz_metaserver.moc"

#include <klistview.h>
#include <qlayout.h>
#include <kpushbutton.h>
#include <qgroupbox.h>

KCMGGZMetaserver::KCMGGZMetaserver(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	KListView *view, *view_servers;
	QVBoxLayout *vbox, *vbox2;
	QHBoxLayout *hbox, *hbox2;
	KPushButton *add, *remove, *autoconfig;
	QGroupBox *box, *box_servers;
	QVBoxLayout *vboxroot;

	box = new QGroupBox("Available meta servers", this);
	box_servers = new QGroupBox("Available game servers", this);

	view = new KListView(box);
	view->addColumn("URI");
	view->addColumn("Supported classes");

	(void)new QListViewItem(view, "ggzmeta://jzaun.com", "ggz");
	(void)new QListViewItem(view, "ggzmeta://ggz.snafu.de", "ggz");
	(void)new QListViewItem(view, "http://www.freeciv.org/metaserver.html", "freeciv");

	add = new KPushButton("Add a server", box);
	remove = new KPushButton("Remove server", box);
	autoconfig = new KPushButton("Autoconfigure", box);

	view_servers = new KListView(box_servers);
	view_servers->addColumn("URI");
	view_servers->addColumn("Type");
	view_servers->addColumn("Description");

	(void)new QListViewItem(view_servers, "ggz://jzaun.com", "GGZ Gaming Zone", "Justin's Developer server");
	(void)new QListViewItem(view_servers, "ggz://ggz.snafu.de", "GGZ Gaming Zone", "GGZ Europe One server");
	(void)new QListViewItem(view_servers, "kmonop://somewhere.org", "Atlantik", "Yet another host");
	(void)new QListViewItem(view_servers, "freeciv://civserver.freeciv.org", "FreeCiv", "SmallPox is cool");

	vboxroot = new QVBoxLayout(this, 5);

	vboxroot->add(box);
	vbox = new QVBoxLayout(box, 15);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(view);
	vbox2 = new QVBoxLayout(hbox, 5);
	vbox2->add(add);
	vbox2->add(remove);
	vbox2->add(autoconfig);

	vboxroot->add(box_servers);
	hbox2 = new QHBoxLayout(box_servers, 15);
	hbox2->add(view_servers);

	load();
}

KCMGGZMetaserver::~KCMGGZMetaserver()
{
}

void KCMGGZMetaserver::load()
{
}

void KCMGGZMetaserver::save()
{
}

const char *KCMGGZMetaserver::caption()
{
	return "Servers";
}

extern "C"
{
	KCMGGZPane *kcmggz_init(QWidget *parent, const char *name)
	{
		return new KCMGGZMetaserver(parent, name);
	}
}

