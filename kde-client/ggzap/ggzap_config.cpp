// GGZap - GGZ quick start application for the KDE panel
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// Header file
#include "ggzap_config.h"

// KDE includes
#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>

// Qt includes
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

// GGZCore++ includes
#include "GGZCoreConfio.h"

// System includes
#include <stdlib.h>

GGZapConfig::GGZapConfig(QWidget *parent, const char *name)
: KDialogBase(parent, name, true, QString::null, KDialogBase::Ok | KDialogBase::Cancel)
{
	QVBoxLayout *vbox;
	QLabel *labelserver, *labelname, *labelgui;
	QFrame *root;

	root = makeMainWidget();

	labelserver = new QLabel(i18n("GGZap Reservation server"), root);
	labelname = new QLabel(i18n("Player name (12 chars maximum)"), root);
	labelgui = new QLabel(i18n("GUI to use"), root);

	m_server = new QLineEdit(root);
	m_username = new QLineEdit(root);

	m_gui = new QComboBox(root);
	m_gui->insertItem(i18n("Blue rectangle"));
	m_gui->insertItem(i18n("Red gear"));

	vbox = new QVBoxLayout(root, 5);
	vbox->add(labelserver);
	vbox->add(m_server);
	vbox->add(labelname);
	vbox->add(m_username);
	vbox->add(labelgui);
	vbox->add(m_gui);

	connect(this, SIGNAL(okClicked()), SLOT(slotAccept()));

	configfile = new QString(getenv("HOME"));
	configfile->append("/.ggz/ggzap.rc");
	configLoad();

	//setFixedSize(200, 150);
	setCaption(i18n("GGZap Configuration"));
	//show();
}

GGZapConfig::~GGZapConfig()
{
	delete configfile;
}

void GGZapConfig::slotAccept()
{
	configSave();
	close();
}

void GGZapConfig::configLoad()
{
	GGZCoreConfio *conf;
	KConfig *config;
	QString type;

	conf = new GGZCoreConfio(configfile->latin1(), GGZCoreConfio::readonly);
	m_server->setText(conf->read("Global", "Server", "ggz.snafu.de"));
	m_username->setText(conf->read("Global", "Username", getenv("USER")));
	delete conf;

	config = kapp->config();
	config->setGroup("GUI");
	type = config->readEntry("Type");

	if(type == "bluebox") m_gui->setCurrentItem(0);
	else if(type == "redgear") m_gui->setCurrentItem(1);
}

void GGZapConfig::configSave()
{
	GGZCoreConfio *conf;
	KConfig *config;
	QString type;

	if(m_username->text().length() > 12) m_username->setText(m_username->text().left(12));
	conf = new GGZCoreConfio(configfile->latin1(), GGZCoreConfio::readwrite | GGZCoreConfio::create);
	conf->write("Global", "Server", m_server->text().latin1());
	conf->write("Global", "Username", m_username->text().latin1());
	conf->commit();
	delete conf;


	type = "bluerectangle";
	if(m_gui->currentItem() == 1) type = "redgear";

	config = kapp->config();
	config->setGroup("GUI");
	config->writeEntry("Type", type);
	config->sync();
}

