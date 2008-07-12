#include "app.h"
#include "plugin.h"

#include <kapplication.h>
#include <kconfig.h>
#include <ksimpleconfig.h>
#include <klocale.h>
#include <qtreewidget.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kinputdialog.h>
#include <kprocess.h>

#include <kdebug.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qdir.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qimage.h>

#include <QFrame>
#include <QPixmap>

#define PREFIX "XXX"

App::App(QWidget *parent)
: KTabWidget(parent)
{
	KStandardDirs d;
	QWidget *tab_general, *tab_plugins, *tab_profile;
	QVBoxLayout *vbox_general, *vbox_plugins, *vbox_profile;
	QHBoxLayout *hbox_profile, *hbox_profile2;
	QPushButton *profile_add;
	QFrame *frame_grubby;
	QMap<QString, QString> pluginnames, plugininfos;

	tab_profile = new QWidget();
	tab_general = new QWidget();
	tab_plugins = new QWidget();

	QPixmap grubby(d.findResource("data", "kgrubby/kgrubby.png"));
	frame_grubby = new QFrame();
	frame_grubby->setFixedSize(grubby.width(), grubby.height());

	QPalette palette;
	palette.setBrush(frame_grubby->backgroundRole(), QBrush(grubby));
	frame_grubby->setPalette(palette);

	m_profile = new QComboBox();

	QDir home = QDir::home();
	home.setPath(home.path() + "/.ggz/kgrubby/");
	QStringList profiles = home.entryList(QDir::Dirs);
	for(QStringList::Iterator it = profiles.begin(); it != profiles.end(); it++)
	{
		if(((*it) == ".") || ((*it) == "..")) continue;
		m_profile->addItem((*it));
	}
	if(m_profile->count() == 0)
		m_profile->addItem("default");
	if(m_profile->count() == 1)
		m_profile->setEnabled(false);
	m_profilename = m_profile->currentText();

	profile_add = new QPushButton(i18n("Add Profile..."));
	m_profile_remove = new QPushButton(i18n("Remove"));
	if(m_profile->count() == 1)
		m_profile_remove->setEnabled(false);

	m_networktypelabel = new QLabel(i18n("Network type"));
	m_networktype = new QComboBox(tab_general);
	m_networktype->addItem(i18n("GGZ Gaming Zone"));
	m_networktype->addItem(i18n("IRC"));
	m_networktype->addItem(i18n("SILC"));
	m_networktype->addItem(i18n("Console"));

	m_namelabel = new QLabel(i18n("Name"));
	m_name = new QLineEdit();
	m_ownerlabel = new QLabel(i18n("Owner"));
	m_owner = new QLineEdit();
	m_langlabel = new QLabel(i18n("Primary language (2-letter)"));
	m_lang = new QLineEdit();
	m_hostlabel = new QLabel(i18n("Hostname"));
	m_host = new QLineEdit();
	m_autojoinlabel = new QLabel(i18n("Autojoin room/channel"));
	m_autojoin = new QLineEdit();

	QStringList headers;
	headers << i18n("Name");
	headers << i18n("Description");

	m_pluginlist = new QTreeWidget();
	m_pluginlist->setHeaderLabels(headers);

	pluginnames["game"] = i18n("Games");
	pluginnames["self"] = i18n("Himself");
	pluginnames["extra"] = i18n("Extras");
	pluginnames["people"] = i18n("People");
	pluginnames["learning"] = i18n("Learning");
	pluginnames["exec"] = i18n("Execution");
	pluginnames["embed"] = i18n("Embedding");
	pluginnames["programming"] = i18n("Programming");
	pluginnames["simplereturn"] = i18n("Echo");
	pluginnames["badword"] = i18n("Bad words");

	plugininfos["game"] = i18n("Ability to play some games");
	plugininfos["self"] = i18n("Self presentation and help");
	plugininfos["extra"] = i18n("Several extra messages");
	plugininfos["people"] = i18n("Player data registration");
	plugininfos["learning"] = i18n("Word database");
	plugininfos["exec"] = i18n("Program execution");
	plugininfos["embed"] = i18n("Scripts execution");
	plugininfos["programming"] = i18n("Comments about languages");
	plugininfos["simplereturn"] = i18n("Test plugin to return every line");
	plugininfos["badword"] = i18n("Looks for nasty words");

	m_plugindialogs["people"] = "people";
	m_plugindialogs["learning"] = "learning";
	m_plugindialogs["exec"] = "exec";
	m_plugindialogs["embed"] = "embed";
	m_plugindialogs["badword"] = "badword";

	QTreeWidgetItem *item;

	QMap<QString, QString>::Iterator it;
	for(it = pluginnames.begin(); it != pluginnames.end(); it++)
	{
		item = new QTreeWidgetItem();
		item->setText(0, it.value());
		item->setText(1, plugininfos[it.key()]);
		m_pluginlist->addTopLevelItem(item);
		m_plugins[item] = it.key();
	}

	m_plugin_configure = new QPushButton(i18n("Configure..."));
	m_plugin_configure->setEnabled(false);

	hbox_profile2 = new QHBoxLayout();
	hbox_profile2->addStretch(1);
	hbox_profile2->addWidget(frame_grubby);
	hbox_profile2->addStretch(1);

	hbox_profile = new QHBoxLayout();
	hbox_profile->addWidget(profile_add);
	hbox_profile->addWidget(m_profile_remove);

	vbox_profile = new QVBoxLayout();
	vbox_profile->addLayout(hbox_profile2);
	vbox_profile->addWidget(m_profile);
	vbox_profile->addLayout(hbox_profile);
	vbox_profile->addStretch(1);

	tab_profile->setLayout(vbox_profile);

	vbox_general = new QVBoxLayout();
	vbox_general->addWidget(m_namelabel);
	vbox_general->addWidget(m_name);
	vbox_general->addWidget(m_ownerlabel);
	vbox_general->addWidget(m_owner);
	vbox_general->addWidget(m_langlabel);
	vbox_general->addWidget(m_lang);
	vbox_general->addWidget(m_hostlabel);
	vbox_general->addWidget(m_host);
	vbox_general->addWidget(m_autojoinlabel);
	vbox_general->addWidget(m_autojoin);
	vbox_general->addWidget(m_networktypelabel);
	vbox_general->addWidget(m_networktype);
	vbox_general->addStretch(1);

	tab_general->setLayout(vbox_general);

	vbox_plugins = new QVBoxLayout();
	vbox_plugins->addWidget(m_pluginlist);
	vbox_plugins->addWidget(m_plugin_configure);

	tab_plugins->setLayout(vbox_plugins);

	addTab(tab_profile, i18n("Profile"));
	addTab(tab_general, i18n("General"));
	addTab(tab_plugins, i18n("Plugins"));

	connect(m_profile, SIGNAL(activated(int)), SLOT(slotProfileChange()));
	connect(profile_add, SIGNAL(clicked()), SLOT(slotProfileNew()));
	connect(m_profile_remove, SIGNAL(clicked()), SLOT(slotProfileDelete()));
	connect(m_plugin_configure, SIGNAL(clicked()), SLOT(slotPluginConfigure()));
	connect(m_pluginlist, SIGNAL(itemSelectionChanged()), SLOT(slotPluginSelected()));

	connect(m_networktype, SIGNAL(activated(int)), SLOT(slotModified()));
	connect(m_name, SIGNAL(textChanged(const QString&)), SLOT(slotModified()));
	connect(m_owner, SIGNAL(textChanged(const QString&)), SLOT(slotModified()));
	connect(m_lang, SIGNAL(textChanged(const QString&)), SLOT(slotModified()));
	connect(m_host, SIGNAL(textChanged(const QString&)), SLOT(slotModified()));
	connect(m_autojoin, SIGNAL(textChanged(const QString&)), SLOT(slotModified()));

	loadProfile();
}

App::~App()
{
}

void App::slotProfileNew()
{
	QString name = KInputDialog::getText(
		i18n("New profile"),
		i18n("Profile name:"));

	if(!name.isEmpty())
	{
		m_profile->addItem(name);
		m_profile->setEditText(name);

		m_profile_remove->setEnabled(true);
		m_profile->setEnabled(true);

		slotProfileChange();
	}
}

void App::slotProfileDelete()
{
	int ret = KMessageBox::questionYesNo(this,
		i18n("Do you really want to remove this profile?"),
		i18n("Remove profile"));

	if(ret == KMessageBox::Yes)
	{
		QDir home = QDir::home();
		home.setPath(home.path() + "/.ggz/kgrubby/");
		home.setPath(home.path() + "/" + m_profilename);

		KProcess proc;
		proc << "rm";
		proc << "-rf";
		proc << home.path().toLatin1();
		proc.start();

		m_profile->removeItem(m_profile->currentIndex());
		if(m_profile->count() == 1)
		{
			m_profile_remove->setEnabled(false);
			m_profile->setEnabled(false);
		}

		m_profilemodified = false;
		emit signalChanged(false);

		slotProfileChange();
	}
}

void App::slotProfileChange()
{
	if(m_profilemodified)
	{
		int ret = KMessageBox::questionYesNo(this,
			i18n("Should the changes be saved before changing the profile?"),
			i18n("Profile change"));
		if(ret == KMessageBox::Yes)
			saveProfile();
	}
	m_profilename = m_profile->currentText();
	loadProfile();
}

void App::slotPluginConfigure()
{
	QTreeWidgetItem *item = static_cast<QTreeWidgetItem*>(m_pluginlist->currentItem());
	if(!item) return;

	QString name = m_plugins[item];
	QString dialog = m_plugindialogs[name];

	QList<QStringList> valuelist;
	QList<QStringList>::iterator it;
	Plugin p(this);

	if(dialog == "people")
	{
		p.addColumn(i18n("Name"));
		p.addColumn(i18n("Last active"));
		p.addColumn(i18n("Last seen"));
		p.addColumn(i18n("First seen"));
		p.addColumn(i18n("Status"));
		p.addColumn(i18n("Contact"));
		p.addColumn(i18n("Language"));
		p.addColumn(i18n("Realname"));
	}
	else if(dialog == "learning")
	{
		p.addColumn(i18n("Word"));
		p.addColumn(i18n("Meaning"));
	}
	else if(dialog == "exec")
	{
		p.addColumn(i18n("Program location"));
	}
	else if(dialog == "embed")
	{
		p.addColumn(i18n("Script location"));
	}
	else if(dialog == "badword")
	{
		p.addColumn(i18n("Bad word"));
	}
	else return;

	valuelist = m_profiledata[dialog];
	for(it = valuelist.begin(); it != valuelist.end(); it++)
		p.addRow((*it));

	p.exec();

	for(int i = 0; i < p.numRows(); i++)
		valuelist.append(p.getRow(i));
	m_profiledata[dialog] = valuelist;

	m_profilemodified = true;
	emit signalChanged(true);
}

void App::slotPluginSelected()
{
	QTreeWidgetItem *item = static_cast<QTreeWidgetItem*>(m_pluginlist->currentItem());
	if(!item)
	{
		m_plugin_configure->setEnabled(false);
		return;
	}

	QString name = m_plugins[item];
	if(!m_plugindialogs[name].isEmpty()) m_plugin_configure->setEnabled(true);
	else m_plugin_configure->setEnabled(false);
}

void App::slotModified()
{
	m_profilemodified = true;
	emit signalChanged(true);
}

void App::saveProfile()
{
	QList<QStringList> valuelist;
	QList<QStringList>::iterator it;
	QFile file;
	QMap<QTreeWidgetItem*, QString>::Iterator pit;

	QDir home = QDir::home();
	home.setPath(home.path() + "/.ggz");
	home.mkdir(home.path());
	home.setPath(home.path() + "/kgrubby");
	home.mkdir(home.path());
	home.setPath(home.path() + "/" + m_profilename);
	home.mkdir(home.path());

	file.setFileName(home.path() + "/grubby.rc");
	if(file.open(QIODevice::WriteOnly))
	{
		QString moduledir = PREFIX "/lib/grubby/modules/";
		QString coremoduledir = PREFIX "/lib/grubby/coremodules/";

		QTextStream stream(&file);
		stream << "[preferences]\n";
		stream << "name = " << m_name->text() << "\n";
		stream << "language = " << m_lang->text() << "\n";
		stream << "owner = " << m_owner->text() << "\n";
		stream << "autojoin = " << m_autojoin->text() << "\n";
		stream << "host = " << m_host->text() << "\n";
		stream << "\n";

		stream << "[modules]\n";
		for(pit = m_plugins.begin(); pit != m_plugins.end(); pit++)
		{
			QString module = pit.value();
			stream << module << " = " << moduledir << "libgurumod_" << module << ".so\n";
		}
		stream << "\n";

		stream << "[guru]\n";
		stream << "netconsole = " << coremoduledir << "libguru_netconsole.so\n";
		stream << "netirc = " << coremoduledir << "libguru_netirc.so\n";
		stream << "netggz = " << coremoduledir << "libguru_netggz.so\n";
		stream << "player = " << coremoduledir << "libguru_player.so\n";
		stream << "i18n = " << coremoduledir << "libguru_i18n.so\n";
		stream << "modules =";
		for(pit = m_plugins.begin(); pit != m_plugins.end(); pit++)
		{
			QTreeWidgetItem *item = pit.key();
			if(item->checkState(0) == Qt::Checked)
			{
				stream << " ";
				stream << pit.value();
			}
		}
		stream << "\n";
		stream << "net = ";
		if(m_networktype->currentText() == i18n("GGZ Gaming Zone"))
			stream << coremoduledir << "libguru_netggz.so\n";
		if(m_networktype->currentText() == i18n("IRC"))
			stream << coremoduledir << "libguru_netirc.so\n";
		if(m_networktype->currentText() == i18n("SILC"))
			stream << coremoduledir << "libguru_netsilc.so\n";
		if(m_networktype->currentText() == i18n("Console"))
			stream << coremoduledir << "libguru_netconsole.so\n";
		stream << "\n";

		file.close();
	}

	home.setPath(home.path() + "/grubby");
	home.mkdir(home.path());

	valuelist = m_profiledata["badword"];
	file.setFileName(home.path() + "/modbadword.rc");
	if(file.open(QIODevice::WriteOnly))
	{
		QTextStream stream(&file);
		stream << "[badwords]\n";
		stream << "badwords =";
		for(it = valuelist.begin(); it != valuelist.end(); it++)
		{
			stream << " ";
			stream << (*it)[0];
		}
		stream << "\n";
		file.close();
	}

	valuelist = m_profiledata["exec"];
	file.setFileName(home.path() + "/modexec.rc");
	if(file.open(QIODevice::WriteOnly))
	{
		QTextStream stream(&file);
		stream << "[programs]\n";
		stream << "programs =";
		for(it = valuelist.begin(); it != valuelist.end(); it++)
		{
			stream << " ";
			stream << (*it)[0];
		}
		stream << "\n";
		file.close();
	}

	valuelist = m_profiledata["embed"];
	file.setFileName(home.path() + "/modembed.rc");
	if(file.open(QIODevice::WriteOnly))
	{
		QTextStream stream(&file);
		stream << "[programs]\n";
		stream << "programs =";
		for(it = valuelist.begin(); it != valuelist.end(); it++)
		{
			stream << " ";
			stream << (*it)[0];
		}
		stream << "\n";
		file.close();
	}

	valuelist = m_profiledata["learning"];
	file.setFileName(home.path() + "/learning.db");
	if(file.open(QIODevice::WriteOnly))
	{
		QTextStream stream(&file);
		for(it = valuelist.begin(); it != valuelist.end(); it++)
		{
			stream << (*it)[0];
			stream << "\t";
			stream << (*it)[1];
			stream << "\n";
		}
		file.close();
	}

	valuelist = m_profiledata["people"];
	file.setFileName(home.path() + "/playerdb");
	if(file.open(QIODevice::WriteOnly))
	{
		QTextStream stream(&file);
		for(it = valuelist.begin(); it != valuelist.end(); it++)
		{
			stream << "[" << (*it)[0] << "]\n";
			stream << "LASTACTIVE = " << (*it)[1] << "\n";
			stream << "LASTSEEN = " << (*it)[2] << "\n";
			stream << "SEEN = " << (*it)[3] << "\n";
			stream << "STATUS = " << (*it)[4] << "\n";
			stream << "CONTACT = " << (*it)[5] << "\n";
			stream << "LANGUAGE = " << (*it)[6] << "\n";
			stream << "REALNAME = " << (*it)[7] << "\n";
			stream << "\n";
		}
		file.close();
	}
	
	m_profilemodified = false;
	emit signalChanged(false);
}

void App::loadProfile()
{
	QList<QStringList> valuelist;
	QList<QStringList>::iterator it;
	QFile file;
	QMap<QTreeWidgetItem*, QString>::Iterator pit;
	QStringList::Iterator lit;
	QString entrystring;
	QStringList entry;
	KConfigGroup cg;

	QDir home = QDir::home();
	home.setPath(home.path() + "/.ggz/kgrubby/");
	home.setPath(home.path() + "/" + m_profilename);

	KConfig conf(home.path() + "/grubby.rc", KConfig::SimpleConfig);
	cg = KConfigGroup(&conf, "preferences");
	m_name->setText(cg.readEntry("name"));
	m_lang->setText(cg.readEntry("language"));
	m_owner->setText(cg.readEntry("owner"));
	m_autojoin->setText(cg.readEntry("autojoin"));
	m_host->setText(cg.readEntry("host"));

	cg = KConfigGroup(&conf, "guru");
	QString activestring = cg.readEntry("modules");
	QStringList active = activestring.split(" ");
	for(pit = m_plugins.begin(); pit != m_plugins.end(); pit++)
	{
		QTreeWidgetItem *item = pit.key();
		if(active.contains(pit.value())) item->setCheckState(0, Qt::Checked);
		else item->setCheckState(0, Qt::Unchecked);
	}
	QString net = cg.readEntry("net");
	if(net.contains("netggz")) m_networktype->setEditText(i18n("GGZ Gaming Zone"));
	else if(net.contains("netirc")) m_networktype->setEditText(i18n("IRC"));
	else if(net.contains("netsilc")) m_networktype->setEditText(i18n("SILC"));
	else if(net.contains("netconsole")) m_networktype->setEditText(i18n("Console"));

	home.setPath(home.path() + "/grubby");

	valuelist.clear();
	KConfig badword_conf(home.path() + "/modbadword.rc", KConfig::SimpleConfig);
	cg = KConfigGroup(&badword_conf, "badwords");
	entrystring = cg.readEntry("badwords");
	entry = entrystring.split(" ");
	for(lit = entry.begin(); lit != entry.end(); lit++)
	{
		QStringList l;
		l << (*lit);
		valuelist.append(l);
	}
	m_profiledata["badword"] = valuelist;

	valuelist.clear();
	KConfig exec_conf(home.path() + "/modexec.rc", KConfig::SimpleConfig);
	cg = KConfigGroup(&exec_conf, "programs");
	entrystring = cg.readEntry("programs");
	entry = entrystring.split(" ");
	for(lit = entry.begin(); lit != entry.end(); lit++)
	{
		QStringList l;
		l << (*lit);
		valuelist.append(l);
	}
	m_profiledata["exec"] = valuelist;

	valuelist.clear();
	KConfig embed_conf(home.path() + "/modembed.rc", KConfig::SimpleConfig);
	cg = KConfigGroup(&embed_conf, "programs");
	entrystring = cg.readEntry("programs");
	entry = entrystring.split(" ");
	for(lit = entry.begin(); lit != entry.end(); lit++)
	{
		QStringList l;
		l << (*lit);
		valuelist.append(l);
	}
	m_profiledata["embed"] = valuelist;

	valuelist.clear();
	file.setFileName(home.path() + "/learning.db");
	if(file.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&file);
		while(!stream.atEnd())
		{
			entrystring = stream.readLine();
			entry = entrystring.split("\t");
			valuelist.append(entry);
		}
		file.close();
	}
	m_profiledata["learning"] = valuelist;

	valuelist.clear();
	KConfig people_conf(home.path() + "/playerdb", KConfig::SimpleConfig);
	QStringList grouplist = people_conf.groupList();
	for(lit = grouplist.begin(); lit != grouplist.end(); lit++)
	{
		cg = KConfigGroup(&people_conf, (*lit));
		QStringList l;
		l << (*lit);
		entrystring = cg.readEntry("LASTACTIVE");
		l << entrystring;
		entrystring = cg.readEntry("LASTSEEN");
		l << entrystring;
		entrystring = cg.readEntry("SEEN");
		l << entrystring;
		entrystring = cg.readEntry("STATUS");
		l << entrystring;
		entrystring = cg.readEntry("CONTACT");
		l << entrystring;
		entrystring = cg.readEntry("LANGUAGE");
		l << entrystring;
		entrystring = cg.readEntry("REALNAME");
		l << entrystring;
		valuelist.append(l);
	}
	m_profiledata["people"] = valuelist;

	m_profilemodified = false;
	emit signalChanged(false);
}

