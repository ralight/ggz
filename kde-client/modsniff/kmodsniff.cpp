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
#include <iostream>

#define DIRPERM (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

using namespace std;

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
	m_ed->setText("The following games have been added:");
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

void KModSniff::addModule(char *modulename, char *frontend)
{
	ofstream mod;
	char modfile[1024];

	sprintf(modfile, "%s/.kde/share/applnk/Games/ggz/games/%s.%s.desktop", getenv("HOME"), modulename, frontend);
	mod.open(modfile);
	if(!mod.is_open()) return;

	mod << "[Desktop Entry]" << endl
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
	DIR *dp;
	struct dirent *ep;
	char moddir[1024];
	char modfile[1024];
	struct stat buf;
	char *token, *token2;
	char *frontend;
	FILE *f;
	char buffer[512];

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
						frontend = strtok(NULL, ".");
						if(token)
						{
							strcpy(modfile, moddir);
							strcat(modfile, "/");
							strcat(modfile, ep->d_name);
							strcat(modfile, ".");
							strcat(modfile, frontend);
							strcat(modfile, ".desktop");
							f = fopen(modfile, "r");
							if(f)
							{
								while(fgets(buffer, sizeof(buffer), f))
								{
									buffer[strlen(buffer) - 1] = 0;
									if(strncmp(buffer, "Exec", 4) == 0)
									{
cout << "debug: token2 (" << buffer << ") in " << modfile << endl;
										token2 = strtok(buffer, " ");
										while(token2)
										{
											if(strcmp(token2, "--frontend") == 0) frontend = strdup(strtok(NULL, " "));
											token2 = strtok(NULL, " ");
										}
									}
								}
							}
							else frontend = "unknown";
cout << "debug: fe is " << frontend << endl;
							list[i].name =(char*)malloc(strlen(token) + 1);
							strcpy(list[i].name, strdup(token));
							//list[i].frontend = (char*)malloc(strlen(frontend) + 1);
							//strcpy(list[i].frontend, frontend);
							list[i].frontend = frontend;
						}
						while(token) token = strtok(NULL, ".");
					}
					i++;
				}
			}
			closedir(dp);
		}
		if(k == 0)
		{
			list = (GGZModuleEntry*)malloc((listcount + 1) * sizeof(GGZModuleEntry));
			list[listcount].name = NULL;
			list[listcount].frontend = NULL;
		}
	}

	//for(int i = 0; i < listcount; i++)
	//	cout << "  LIST " << list[i] << endl;

	return list;
}

