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
	QLabel *title;
	QLabel *description;
	QVBoxLayout *vbox;
	QPushButton *ok;
	QLabel *server;

	title = new QLabel(i18n("<b>Global Settings</b>"), this);
	description = new QLabel(i18n("Please specify some environment variables here."), this);

	server = new QLabel(i18n("Path to ggzd"), this);
	m_startup = new QCheckBox(i18n("Show connection dialog on startup"), this);

	m_server = new QLineEdit(this);

	ok = new QPushButton("OK", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(title);
	vbox->add(description);
	vbox->add(server);
	vbox->add(m_server);
	vbox->add(m_startup);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccept()));

	setFixedSize(300, 150);
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
	config->commit();

	delete config;

	close();
}

void KGGZPrefEnv::loadSettings()
{
	GGZCoreConfio *config;
	char *server;
	int startup;

	config = new GGZCoreConfio(KGGZCommon::append(getenv("HOME"), "/.ggz/kggz.rc"), GGZCoreConfio::readwrite | GGZCoreConfio::create);
	KGGZCommon::clear();

	server = config->read("Environment", "Server", "/usr/bin/ggzd");
	startup = config->read("Preferences", "Showdialog", 0);

	m_server->setText(server);
	m_startup->setChecked(startup);

	delete config;
}

