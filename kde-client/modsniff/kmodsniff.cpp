// Modsniff includes
#include "kmodsniff.h"
#include "modsniff.h"

// KDE includes
#include <klocale.h>
#include <kstandarddirs.h>

// Qt includes
#include <qlabel.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qdir.h>
#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qtextstream.h>

// System includes
#include <stdlib.h>

using namespace std;

KModSniff::KModSniff(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *label;
	QPushButton *ok;

	ok = new QPushButton(i18n("Exit"), this);

	m_search = new QPushButton(i18n("Find games"), this);

	label = new QLabel(i18n("This tool searches for new GGZ games\n"
		"and enters them into the quick start menu.\n"
		"The following games have been found:"), this);

	m_ed = new QMultiLineEdit(this);
	m_ed->setText(i18n("Press 'Find games' to search for GGZ compliant games."));

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label);
	vbox->add(m_ed);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(m_search);
	hbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(close()));
	connect(m_search, SIGNAL(clicked()), SLOT(slotSearch()));

	resize(400, 200);
	setCaption(i18n("GGZ Module Sniffer"));
	show();
}

KModSniff::~KModSniff()
{
}

void KModSniff::slotSearch()
{
	GGZModuleEntry *list;
	GGZModuleEntry *installed;
	int i;
	int ret;

	m_search->setEnabled(FALSE);

	installed = installedModules();

	ret = modsniff_init();
	if(ret != 0)
	{
		m_search->setEnabled(TRUE);
		return;
	}

	list = modsniff_list();
	list = modsniff_merge(installed);

	i = 0;
	m_ed->setText(i18n("The following games have been added:"));
	while((list) && (list[i].name))
	{
		m_ed->append(QString("%1 (%2)").arg(list[i].name).arg(list[i].frontend));
		addModule(list[i].name, list[i].frontend);
		i++;
	}
	if(!i) m_ed->setText(i18n("No new games found. All GGZ games are already configured."));

	modsniff_end();

	m_search->setEnabled(TRUE);
}

void KModSniff::addModule(const char *modulename, const char *frontend)
{
	QString filename, modfile;

	filename = QString("Games/ggz/games/%1.%2.desktop").arg(modulename).arg(frontend);
	modfile = locateLocal("apps", filename);

	QFile mod(modfile);
	if(!mod.open(IO_ReadWrite)) return;

	QTextStream mts(&mod);
	mts << "[Desktop Entry]" << endl
		<< "Comment=" << endl
		<< "Exec=ggzap --module " << modulename << " --frontend " << frontend << endl
		<< "Icon=ggzquick.png" << endl
		<< "MimeType=" << endl
		<< "Name=" << modulename << " (" << frontend << ")" << endl
		<< "Path=" << endl
		<< "ServiceTypes=" << endl
		<< "SwallowExec=" << endl
		<< "SwallowTitle=" << endl
		<< "Terminal=false" << endl
		<< "TerminalOptions=" << endl
		<< "Type=Application" << endl
		<< "X-KDE-SubstituteUID=false" << endl
		<< "X-KDE-Username=" << endl;

	mod.close();
}

GGZModuleEntry *KModSniff::installedModules()
{
	GGZModuleEntry *list, *bak;
	int listcount, i;
	KStandardDirs d;
	QString moddir;
	QStringList bufferlist, buffer;
	QDir dir;

	list = NULL;
	bak = NULL;
	listcount = 0;

	QString path = "Games/ggz/games";

	moddir = locateLocal("apps", path);
	if(!d.exists(moddir + "/.modsniff_info"))
		locateLocal("apps", path + "/.modsniff_info");

	dir = QDir(moddir, "*.desktop");
	bufferlist = dir.entryList();
	listcount = bufferlist.count();
	if(listcount > 0)
	{
		i = 0;
		list = (GGZModuleEntry*)malloc((listcount + 1) * sizeof(GGZModuleEntry));
		list[listcount].name = NULL;
		list[listcount].frontend = NULL;

		for(QStringList::Iterator it = bufferlist.begin(); it != bufferlist.end(); it++)
		{
			buffer = buffer.split(".", (*it));
			if(buffer.count() > 2)
			{
				list[i].name = strdup((*(buffer.at(0))).latin1());
				list[i].frontend = strdup((*(buffer.at(1))).latin1());
			}
			else
			{
				list[i].frontend = NULL;
				list[i].name = NULL;
			}
			i++;
		}
	}

	return list;
}

