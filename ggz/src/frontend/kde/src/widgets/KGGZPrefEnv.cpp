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
// KGGZPrefEnv: Ask users for paths to programs and global configuration settings. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZPrefEnv.h"

// KGGZ includes
#include "KGGZCommon.h"
#include "KGGZCaption.h"

// GGZCore++ includes
#include "GGZCoreConfio.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

// System includes
#include <stdlib.h>

// Constructor
KGGZPrefEnv::KGGZPrefEnv(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	KGGZCaption *title;
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QPushButton *ok, *cancel;
	QLabel *server;

	title = new KGGZCaption(i18n("Global Settings"), i18n("Please specify some environment variables here."), this);

	server = new QLabel(i18n("Path to ggzd"), this);
	m_startup = new QCheckBox(i18n("Show connection dialog on startup"), this);
	m_chatlog = new QCheckBox(i18n("Log chat conversation"), this);
	m_speech = new QCheckBox(i18n("Enable text-to-speech (rsynth)"), this);

	m_server = new QLineEdit(this);

	ok = new QPushButton(i18n("Save configuration"), this);
	cancel = new QPushButton(i18n("Cancel"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(title);
	vbox->add(server);
	vbox->add(m_server);
	vbox->add(m_startup);
	vbox->add(m_chatlog);
	vbox->add(m_speech);

	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(ok);
	hbox->add(cancel);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccept()));
	connect(cancel, SIGNAL(clicked()), SLOT(close()));

	setFixedSize(300, 210);
	setCaption(i18n("Global Settings"));
	show();

	loadSettings();
}

// Destructor
KGGZPrefEnv::~KGGZPrefEnv()
{
}

void KGGZPrefEnv::slotAccept()
{
	GGZCoreConfio *config;

	config = new GGZCoreConfio(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"), GGZCoreConfio::readwrite | GGZCoreConfio::create);
	KGGZCommon::clear();

	config->write("Environment", "Server", m_server->text().latin1());
	config->write("Preferences", "Showdialog", m_startup->isChecked());
	config->write("Preferences", "Chatlog", m_startup->isChecked());
	config->write("Preferences", "Speech", m_startup->isChecked());
	config->commit();

	delete config;

	close();
}

void KGGZPrefEnv::loadSettings()
{
	GGZCoreConfio *config;
	char *server;
	int startup, chatlog, speech;

	config = new GGZCoreConfio(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"), GGZCoreConfio::readwrite | GGZCoreConfio::create);
	KGGZCommon::clear();

	server = config->read("Environment", "Server", "/usr/bin/ggzd");
	startup = config->read("Preferences", "Showdialog", 0);
	chatlog = config->read("Preferences", "Chatlog", 0);
	speech = config->read("Preferences", "Speech", 0);

	m_server->setText(server);
	m_startup->setChecked(startup);
	m_chatlog->setChecked(chatlog);
	m_speech->setChecked(speech);

	delete config;
}

