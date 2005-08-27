/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZPrefEnv: Ask users for paths to programs and global configuration settings. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZPrefEnv.h"

// KGGZ includes
#include "KGGZCommon.h"
#include "KGGZCaption.h"
#include "KGGZLineSeparator.h"

// GGZCore++ includes
#include "GGZCoreConfio.h"

// KDE includes
#include <klocale.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kurl.h>

// Qt includes
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qdir.h>

// System includes
#include <stdlib.h>

// Constructor
KGGZPrefEnv::KGGZPrefEnv(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	KGGZCaption *title;
	QVBoxLayout *vbox, *vbox1, *vbox2, *vbox3;
	QHBoxLayout *hbox, *hbox2, *hbox3, *hbox4, *hbox5;
	QPushButton *ok, *cancel;
	QLabel *server, *country, *playername, *email, *homepage;
	KGGZLineSeparator *sep;
	QTabWidget *tabwidget;
	QWidget *tab1, *tab2, *tab3;

	title = new KGGZCaption(i18n("Global Settings"), i18n("Please specify some environment variables here."), this);

	tabwidget = new QTabWidget(this);
	tab1 = new QWidget(tabwidget);
	tab2 = new QWidget(tabwidget);
	tab3 = new QWidget(tabwidget);
	tabwidget->addTab(tab1, i18n("General"));
	tabwidget->addTab(tab2, i18n("Personal"));
	tabwidget->addTab(tab3, i18n("Chat"));

	server = new QLabel(i18n("Path to ggzd"), tab1);
	m_startup = new QCheckBox(i18n("Show connection dialog on startup"), tab1);
	m_chatlog = new QCheckBox(i18n("Log chat conversation"), tab1);
	m_speech = new QCheckBox(i18n("Enable text-to-speech (rsynth)"), tab1);
	m_motd = new QCheckBox(i18n("Display MOTD upon login"), tab1);

	m_server = new QLineEdit(tab1);

	country = new QLabel(i18n("Country"), tab2);
	playername = new QLabel(i18n("Full name"), tab2);
	email = new QLabel(i18n("Email"), tab2);
	homepage = new QLabel(i18n("Homepage"), tab2);

	m_playername = new QLineEdit(tab2);
	m_homepage = new QLineEdit(tab2);
	m_email = new QLineEdit(tab2);

	countrybox = new QComboBox(tab2);

	m_timestamps = new QCheckBox(i18n("Time stamps in chat"), tab3);

	sep = new KGGZLineSeparator(this);

	ok = new QPushButton(i18n("Save configuration"), this);
	cancel = new QPushButton(i18n("Cancel"), this);

	vbox1 = new QVBoxLayout(tab1, 5);
	vbox1->add(server);
	vbox1->add(m_server);
	vbox1->add(m_startup);
	vbox1->add(m_chatlog);
	vbox1->add(m_speech);
	vbox1->add(m_motd);
	vbox2 = new QVBoxLayout(tab2, 5);
	hbox3 = new QHBoxLayout(vbox2, 5);
	hbox3->add(playername);
	hbox3->add(m_playername);
	hbox4 = new QHBoxLayout(vbox2, 5);
	hbox4->add(email);
	hbox4->add(m_email);
	hbox5 = new QHBoxLayout(vbox2, 5);
	hbox5->add(homepage);
	hbox5->add(m_homepage);
	hbox2 = new QHBoxLayout(vbox2, 5);
	hbox2->add(country);
	hbox2->add(countrybox);
	vbox3 = new QVBoxLayout(tab3, 5);
	vbox3->add(m_timestamps);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(title);
	vbox->add(tabwidget);
	vbox->add(sep);

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(ok);
	hbox->add(cancel);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(cancel, SIGNAL(clicked()), SLOT(close()));

	resize(340, 260);
	setCaption(i18n("Global Settings"));
	show();

	loadCountries();
	loadSettings();
}

// Destructor
KGGZPrefEnv::~KGGZPrefEnv()
{
}

void KGGZPrefEnv::slotAccept()
{
	GGZCoreConfio *config;

	config = new GGZCoreConfio(QString("%1/.ggz/kggz.rc").arg(QDir::homeDirPath()),
		GGZCoreConfio::readwrite | GGZCoreConfio::create);

	config->write("Environment", "Server", m_server->text().utf8());
	config->write("Preferences", "Showdialog", m_startup->isChecked());
	config->write("Preferences", "Chatlog", m_chatlog->isChecked());
	config->write("Preferences", "Timestamps", m_timestamps->isChecked());
	config->write("Preferences", "Speech", m_speech->isChecked());
	config->write("Preferences", "MOTD", m_motd->isChecked());
	config->write("Personal", "Country", countrybox->currentText().utf8());
	config->write("Personal", "Name", m_playername->text().utf8());
	config->write("Personal", "Email", m_email->text().utf8());
	config->write("Personal", "Homepage", m_homepage->text().utf8());
	config->commit();

	delete config;

	emit signalAccepted();
	//close();
}

void KGGZPrefEnv::loadSettings()
{
	GGZCoreConfio *config;
	char *server;
	int startup, chatlog, speech, motd, timestamps;
	char *homepage, *email, *name, *country;

	config = new GGZCoreConfio(QString("%1/.ggz/kggz.rc").arg(QDir::homeDirPath()),
		GGZCoreConfio::readwrite | GGZCoreConfio::create);

	server = config->read("Environment", "Server", "/usr/bin/ggzd");
	startup = config->read("Preferences", "Showdialog", 0);
	chatlog = config->read("Preferences", "Chatlog", 0);
	speech = config->read("Preferences", "Speech", 0);
	motd = config->read("Preferences", "MOTD", 1);
	timestamps = config->read("Preferences", "Timestamps", 0);

	country = config->read("Personal", "Country", "");
	name = config->read("Personal", "Name", "");
	email = config->read("Personal", "Email", "");
	homepage = config->read("Personal", "Homepage", "");

	m_server->setText(server);
	m_startup->setChecked(startup);
	m_chatlog->setChecked(chatlog);
	m_speech->setChecked(speech);
	m_motd->setChecked(motd);
	m_timestamps->setChecked(timestamps);

	m_playername->setText(name);
	m_email->setText(email);
	m_homepage->setText(homepage);
	for(int i = 0; i < countrybox->count(); i++)
		if(countrybox->text(i) == country)
		{
			countrybox->setCurrentItem(i);
			break;
		}

	delete config;
}

void KGGZPrefEnv::loadCountries()
{
	KStandardDirs d;
	KURL u;
	QStringList list;
	QString name, region, path;

	/*list = d.findAllResources("locale", QString("l10n/%1").arg("*.desktop"));
	for(QStringList::iterator it = list.begin(); it != list.end(); it++)
	{
		KSimpleConfig conf((*it));
		conf.setGroup("KCM Locale");
		name = conf.readEntry("Name");
		countrybox->insertItem(name);
	}*/

	list = d.findAllResources("locale", "l10n/*/*.desktop");
	for(QStringList::iterator it = list.begin(); it != list.end(); it++)
	{
		KSimpleConfig conf((*it));
		conf.setGroup("KCM Locale");
		name = conf.readEntry("Name");
		region = conf.readEntry("Region");
		u = KURL((*it));
		path = u.directory() + "/flag.png";
		countrybox->insertItem(QPixmap(path), name /*+ " - - " + region*/);
	}
}

