#include "kmodsniff.h"
#include "modsniff.h"

#ifdef __STRICT_ANSI__
#ifndef __USE_BSD
#define __USE_BSD
#include <sys/stat.h>
#endif
#ifndef strdup
#define strdup(x) strcpy(((char*)malloc(strlen(x) + 1)), x)
#endif
#endif

#include <klocale.h>

#include <qlabel.h>
#include <qlayout.h>

#include <fstream>
#include <stdlib.h>
#include <cstdio>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <string.h>

#define DIRPERM (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

KModSniff::KModSniff(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *label;
	QPushButton *ok;

	ok = new QPushButton("Exit", this);

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

	setFixedSize(300, 200);
	setCaption("GGZ ModSniffer");
	show();
}

KModSniff::~KModSniff()
{
}

void KModSniff::slotSearch()
{
	char **list;
	char **installed;
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
	m_ed->setText("The following games have been added:");
	while((list) && (list[i]))
	{
		m_ed->append(list[i]);
		addModule(list[i]);
		i++;
	}
	if(!i) m_ed->setText(i18n("No new games found. All GGZ games are already configured."));

	modsniff_end();

	m_search->setEnabled(TRUE);
}

void KModSniff::addModule(char *modulename)
{
	ofstream mod;
	char modfile[1024];

	sprintf(modfile, "%s/.kde/share/applnk/Games/ggz/games/%s.desktop", getenv("HOME"), modulename);
	mod.open(modfile);
	if(!mod.is_open()) return;

	mod << "[Desktop Entry]" << endl
		<<" Comment[de]=" << endl
		<< "Exec=ggzap --module " << modulename << endl
		<< "Icon=ggzquick.png" << endl
		<< "MimeType=" << endl
		<< "Name[de]=" << modulename << endl
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

char **KModSniff::installedModules()
{
	char **list, **bak;
	int listcount, i;
	DIR *dp;
	struct dirent *ep;
	char moddir[1024];
	struct stat buf;
	char *token;

	list = NULL;
	bak = NULL;
	listcount = 0;

	sprintf(moddir, "%s/.kde/share/applnk/Games/ggz/games", getenv("HOME"));
	stat(moddir, &buf);
	if((buf.st_mode & S_IFDIR) == 0)
	{
		sprintf(moddir, "%s/.kde", getenv("HOME"));
		mkdir(moddir, DIRPERM);
		strcat(moddir, "/share");
		mkdir(moddir, DIRPERM);
		strcat(moddir, "/applnk");
		mkdir(moddir, DIRPERM);
		strcat(moddir, "/Games");
		mkdir(moddir, DIRPERM);
		strcat(moddir, "/ggz");
		mkdir(moddir, DIRPERM);
		strcat(moddir, "/games");
		mkdir(moddir, DIRPERM);
	}

	// Won't work :(
	/*dp = opendir(moddir);
	if(dp)
	{
		while(ep = readdir(dp))
		{
			if(!fnmatch("*.desktop", ep->d_name, FNM_NOESCAPE))
			{
				cout << "found installed: " << ep->d_name << endl;
				bak = list;
				listcount++;
				list = (char**)malloc(listcount + 1);
				for(int i = 0; i < listcount - 1; i++)
					list[i] = bak[i];
				list[listcount - 1] = strdup(ep->d_name);
				if(bak) free(bak);

				for(int i = 0; i < listcount; i++)
					cout << "list " << list[i] << endl;
			}
		}
		closedir(dp);
	}*/

	for(int k = 0; k < 2; k++)
	{
		i = 0;
		dp = opendir(moddir);
		if(dp)
		{
			while((ep = readdir(dp)) != 0)
			{
				if(!fnmatch("*.desktop", ep->d_name, FNM_NOESCAPE))
				{
					if(k == 0) listcount++;
					else if(i <= listcount)
					{
						token = strtok(ep->d_name, ".");
						list[i] = strdup(token);
						while(token) token = strtok(NULL, ".");
					}
					i++;
				}
			}
			closedir(dp);
		}
		if(k == 0)
		{
			list = (char**)malloc((listcount + 1) * sizeof(char*));
			list[listcount] = NULL;
		}
	}

	//for(int i = 0; i < listcount; i++)
	//	cout << "  LIST " << list[i] << endl;

	return list;
}

