/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4                //
//    Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net      //
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
// KGGZConnect: Display a connection dialog which allows in-place server addition. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZConnect.h"

// KGGZ includes
#include "KGGZCommon.h"
#include "KGGZLineSeparator.h"
#include "KGGZCaption.h"

// KDE classes
#include <kmessagebox.h>
#include <klocale.h>

// Qt classes
#include <qlayout.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qvalidator.h>

// System includes
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// GGZCore++ includes
#include "GGZCoreConfio.h"

/* Constructor: set up a small dialog for connections; provide server profile list */
KGGZConnect::KGGZConnect(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox1, *vbox2;
	QHBoxLayout *hbox1, *hbox2, *hbox3, *hbox4;
	QPushButton *button_ok, *button_cancel;
	QLabel *label_host, *label_port, *label_name, *label_password;
	QRadioButton *mode_normal, *mode_guest, *mode_firsttime;
	QLabel *label_mode;
	QIntValidator *valid;
	KGGZLineSeparator *sep1, *sep2;
	KGGZCaption *caption;

	KGGZDEBUGF("KGGZConnect::KGGZConnect()\n");

	m_input = NULL;

	profile_select = new QComboBox(this);
	profile_new = new QPushButton(i18n("Create new profile"), this);
	profile_delete = new QPushButton(i18n("Delete profile"), this);
	profile_edit = new QPushButton(i18n("<< Edit"), this);

	m_pane = new QWidget(this);

	caption = new KGGZCaption(i18n("Connection"),
		i18n("Select your favorite connection profile\n"
		"and connect to a game server."), this);

	label_host = new QLabel(i18n("Server host:"), m_pane);
	label_port = new QLabel(i18n("Server port:"), m_pane);
	label_name = new QLabel(i18n("Log in as user/nickname:"), m_pane);
	label_password = new QLabel(i18n("Use password:"), m_pane);

	valid = new QIntValidator(this);
	valid->setRange(1024, 32767);

	input_host = new QLineEdit(m_pane);
	input_port = new QLineEdit(m_pane);
	input_port->setValidator(valid);
	input_name = new QLineEdit(m_pane);
	input_password = new QLineEdit(m_pane);
	input_password->setEchoMode(QLineEdit::Password);

	button_ok = new QPushButton(i18n("Connect"), this);
	button_cancel = new QPushButton(i18n("Cancel"), this);

	label_mode = new QLabel(i18n("Log in as:"), m_pane);
	group_mode = new QButtonGroup(m_pane);
	group_mode->hide();
	mode_normal = new QRadioButton(i18n("normal"), m_pane);
	mode_guest = new QRadioButton(i18n("guest"), m_pane);
	mode_firsttime = new QRadioButton(i18n("starter"), m_pane);
	group_mode->insert(mode_normal);
	group_mode->insert(mode_guest);
	group_mode->insert(mode_firsttime);
	mode_normal->setChecked(TRUE);

	option_server = new QCheckBox(i18n("Start ggzd server locally"), this);
	option_tls = new QCheckBox(i18n("Use secure connection (TLS)"), this);

	sep1 = new KGGZLineSeparator(this);
	sep2 = new KGGZLineSeparator(m_pane);

	vbox1 = new QVBoxLayout(this, 5);
	vbox1->add(caption);

	hbox2 = new QHBoxLayout(vbox1, 5);
	hbox2->add(profile_select);

	hbox4 = new QHBoxLayout(vbox1, 5);
	hbox4->add(profile_new);
	hbox4->add(profile_delete);
	hbox4->add(profile_edit);

	vbox1->add(sep1);

	vbox2 = new QVBoxLayout(m_pane, 5);
	vbox2->add(label_host);
	vbox2->add(input_host);
	vbox2->add(label_port);
	vbox2->add(input_port);
	vbox2->add(label_name);
	vbox2->add(input_name);
	vbox2->add(label_password);
	vbox2->add(input_password);

	hbox3 = new QHBoxLayout(vbox2, 5);
	hbox3->add(label_mode);
	hbox3->add(mode_normal);
	hbox3->add(mode_guest);
	hbox3->add(mode_firsttime);

	vbox2->add(sep2);

	vbox1->add(m_pane);

	vbox1->add(option_server);
	vbox1->add(option_tls);

	hbox1 = new QHBoxLayout(vbox1, 5);
	hbox1->add(button_ok);
	hbox1->add(button_cancel);

	//connect(profile_edit, SIGNAL(clicked()), SLOT(edit()));
	connect(button_cancel, SIGNAL(clicked()), SLOT(close()));
	connect(button_ok, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(group_mode, SIGNAL(clicked(int)), SLOT(slotModes(int)));
	connect(option_server, SIGNAL(clicked()), SLOT(slotInvoke()));
	connect(profile_select, SIGNAL(activated(int)), SLOT(slotLoadProfile(int)));
	connect(profile_new, SIGNAL(clicked()), SLOT(slotProfileNew()));
	connect(profile_delete, SIGNAL(clicked()), SLOT(slotProfileDelete()));
	connect(profile_edit, SIGNAL(clicked()), SLOT(slotPane()));

	m_connected = -1;
	m_loginmode = 0;
	m_nosafe = 0;

	setCaption(i18n("Connect to server"));
	//resize(300, 260);

	KGGZDEBUGF("KGGZConnect::KGGZConnect() ready\n");

	slotLoadProfile(-1);
}

/* Destructor */
KGGZConnect::~KGGZConnect()
{
	KGGZDEBUGF("KGGZConnect::~KGGZConnect()\n");
}

void KGGZConnect::slotSaveProfile()
{
	GGZCoreConfio *config;
	//char *defaultserver;
	char *current;

	KGGZDEBUGF("KGGZConnect::slotSaveProfile()\n");

	config = new GGZCoreConfio(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"), GGZCoreConfio::readwrite | GGZCoreConfio::create);
	KGGZCommon::clear();

	//defaultserver = config->read("Session", "Defaultserver", i18n("Default server"));

	// Save previous profile and make new one the default (although it's empty)
	KGGZDEBUG("** save profile\n");
	current = (char*)m_current.latin1();
	config->write(current, "Host", input_host->text());
	config->write(current, "Port", input_port->text());
	config->write(current, "Login", input_name->text());
	config->write(current, "Password", input_password->text());
	config->write(current, "Type", m_loginmode);
	config->write("Session", "Defaultserver", profile_select->currentText());
	config->commit();
	delete config;

	KGGZDEBUGF("KGGZConnect::slotSaveProfile() ready\n");
}

/* Loads a specified server profile, or the first if profile is -1 */
void KGGZConnect::slotLoadProfile(int profile)
{
	GGZCoreConfio *config;
	char **list = NULL;
	int i;
	char *host = NULL;
	char *port = NULL;
	char *username = NULL;
	char *password = NULL;
	int type;
	const char *listentry = NULL;

	KGGZDEBUGF("KGGZConnect::slotLoadProfile()\n");

	if((profile != -1) && (!m_nosafe)) slotSaveProfile();
	m_nosafe = 0;

	config = new GGZCoreConfio(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"), GGZCoreConfio::readonly);
	KGGZCommon::clear();

	/*profile_edit->setEnabled(TRUE);*/

	// setup entries; try to find suitable first entry
	if(profile == -1)
	{
		profile_select->clear();
		listentry = config->read("Session", "Defaultserver", i18n("Default server"));
		profile_select->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/server.png"), listentry);
		KGGZDEBUG("Cleared profile_select. Rereading entries.\n");
		config->read("Servers", "Servers", &i, &list);
		KGGZDEBUG("Done. Now putting them in place. Found %i entries.\n", i);
		for(int j = 0; j < i; j++)
		{
			if(strcmp(list[j], listentry) != 0)
			{
				if(strcmp(list[j], "") != 0)
				{
					KGGZDEBUG("* insert item %s (%i)\n", list[j], j);
					profile_select->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/server.png"), list[j]);
				}
			}
			free(list[j]);
		}
		if(list)
		{
			KGGZDEBUG("List exists, deleting it.\n");
			free(list);
		}
		if(i == 0)
		{
			KGGZDEBUG("None found, using default server.\n");
			modifyServerList(i18n("Default server"), 1);
			/*profile_select->insertItem(i18n("Default server"));*/
		}
	}
	else
		listentry = profile_select->text(profile);

	// read values for selected server
	KGGZDEBUG("slotLoadProfile: read values in %s...\n", listentry);
	host = config->read(listentry, "Host", "ggz.snafu.de");
	port = config->read(listentry, "Port", "5688");
	username = config->read(listentry, "Login", "zocker");
	password = config->read(listentry, "Password", "");
	type = config->read(listentry, "Type", 1);

	// put values into fields
	KGGZDEBUG("slotLoadProfile: set text...\n");
	input_host->setText(host);
	input_port->setText(port);
	input_name->setText(username);
	input_password->setText(password);
	slotModes(type);

	// free allocated memory
	KGGZDEBUG("slotLoadProfile: Freeing memory...\n");
	if(host) free(host);
	if(port) free(port);
	if(username) free(username);
	if(password) free(password);
	//if(listentry) free(listentry);

	// get rid of the configuration object
	delete config;

	m_current = profile_select->currentText();

	// Make sure even default entries are written
	slotSaveProfile();

	KGGZDEBUGF("KGGZConnect::slotLoadProfile() ready\n");
}

/* Start a connection */
void KGGZConnect::slotAccept()
{
	slotSaveProfile();
	close();
	emit signalConnect(input_host->text().latin1(), atoi(input_port->text().latin1()), input_name->text().latin1(), input_password->text().latin1(), m_loginmode);
	//close();
}

void KGGZConnect::slotModes(int loginmode)
{
	m_loginmode = loginmode;
	group_mode->setButton(loginmode);
	if(loginmode == 0)
		input_password->setEnabled(TRUE);
	else
		input_password->setEnabled(FALSE);
}

void KGGZConnect::slotInvoke()
{
	if(option_server->isChecked())
		input_host->setEnabled(FALSE);
	else
		input_host->setEnabled(TRUE);
}

void KGGZConnect::slotProfileNew()
{
	// destroy input window to avoid duplicate signals
	if(m_input)
	{
		KGGZDEBUG("Delete input box now!\n");
		delete m_input;
		m_input = NULL;
	}

	// show input box and connect signal to slot
	if(!m_input) m_input = new KGGZInput(NULL, "KGGZInput");
	m_input->show();
	connect(m_input, SIGNAL(signalText(const char*)), SLOT(slotProfileProcess(const char*)));
}

void KGGZConnect::slotProfileDelete()
{
	KGGZDEBUG("delete current entry =======================> %s\n", m_current.latin1());
	modifyServerList(m_current.latin1(), 2);
	KGGZDEBUG("revoke default server ======================> %i :: %s :: %s\n",
		profile_select->currentItem(), profile_select->text(profile_select->currentItem()).latin1(), profile_select->currentText().latin1());
	profile_select->removeItem(profile_select->currentItem());
	KGGZDEBUG("so we have now =============================> %i :: %s :: %s\n",
		profile_select->currentItem(), profile_select->text(profile_select->currentItem()).latin1(), profile_select->currentText().latin1());
	KGGZDEBUG("load list anew\n");
	m_nosafe = 1;
	slotLoadProfile(profile_select->currentItem());
}

void KGGZConnect::slotProfileProcess(const char *identifier)
{
	KGGZDEBUGF("KGGZConnect::slotProfileProcess()\n");

	// Save current profile
	slotSaveProfile();

	// Add new profile
	profile_select->insertItem(QPixmap(KGGZ_DIRECTORY "/images/icons/server.png"), identifier, 0);
	profile_select->setCurrentItem(0);
	modifyServerList(identifier, 1);
	m_current = QString(identifier);
	input_host->setText("");
	input_name->setText("");
	input_port->setText("5688");
	input_password->setText("");

	// Save it (although is is empty)
	slotSaveProfile();

	KGGZDEBUGF("slotProfileProcess() ready\n");
}

// mode: 1 = add, 2 = delete
void KGGZConnect::modifyServerList(const char *server, int mode)
{
	GGZCoreConfio *config;
	char **list = NULL;
	char **list2 = NULL;
	int i;
	int number;

	KGGZDEBUGF("KGGZConnect::modifyServerList(%s, %i)\n", server, mode);

	config = new GGZCoreConfio(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"), GGZCoreConfio::readwrite | GGZCoreConfio::create);
	KGGZCommon::clear();

	// Update the list of available servers
	KGGZDEBUG("** update server list\n");
	config->read("Servers", "Servers", &i, &list);
	KGGZDEBUG("** found %i entries\n", i);

	list2 = (char**)malloc((i + 2) * sizeof(char*));
	for(int j = 0; j < i + 2; j++)
		list2[j] = NULL;

	number = 0;
	for(int j = 0; j < i; j++)
	{
		if(strcmp(list[j], server) || (mode == 1))
		{
			list2[number] = (char*)malloc(strlen(list[j]) + 1);
			strcpy(list2[number], list[j]);
			number++;
			KGGZDEBUG("** update: %i - %s\n", number - 1, list[j]);
		}
	}
	KGGZDEBUG("** update server list - 1.5\n");
	if(mode == 1)
	{
		KGGZDEBUG("** update server list - 1: %i - %s\n", number, server);
		list2[number] = (char*)malloc(strlen(server) + 1);
		strcpy(list2[number], server);
		number++;
	}
	//list2[number] = NULL;
	KGGZDEBUG("** update server list - 2 (write %i server)\n", number);
	config->write("Servers", "Servers", number, list2);
	if(mode == 2)
	{
		KGGZDEBUG("!!! Remove Section: [%s]\n", server);
		config->removeSection(server);
		if(number == 0)
		{
			KGGZDEBUG("!!! Remove key: Servers\n");
			config->removeKey("Servers", "Servers");
		}
	}

	config->commit();
	delete config;

	KGGZDEBUG("** update server list - 3\n");
	for(int j = 0; j < i; j++)
	{
		KGGZDEBUG("--free(list): %i - %s\n", j, list[j]);
		free(list[j]);
	}
	for(int j = 0; j < number; j++)
	{
		KGGZDEBUG("--free(list2): %i - %s\n", j, list2[j]);
		free(list2[j]);
	}
	KGGZDEBUG("** update server list - 4\n");
	if(list) free(list);
	KGGZDEBUG("** update server list - 4.5\n");
	// WARNING!!!!! TODO!!!!! SECURITY BUG!!!!!!! MEMORY HOLE!!!!!
	if(list2) free(list2);
	KGGZDEBUG("** update server list - 5\n");
}

void KGGZConnect::slotPane()
{
	if(m_pane->isVisible())
	{
		m_pane->hide();
		profile_edit->setText(i18n("Edit >>"));
		//setFixedHeight(minimumSizeHint().height());
		//resize(50, 50);
		//adjustSize();
		setFixedHeight(100);
	}
	else
	{
		m_pane->show();
		profile_edit->setText(i18n("<< Edit"));
		setFixedHeight(minimumSizeHint().height());
	}
}

int KGGZConnect::optionServer()
{
	return option_server->isChecked();
}

int KGGZConnect::optionSecure()
{
	return option_tls->isChecked();
}

