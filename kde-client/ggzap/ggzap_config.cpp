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

// Qt includes
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

// GGZCore++ includes
#include "GGZCoreConfio.h"

// System includes
#include <stdlib.h>

GGZapConfig::GGZapConfig(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox;
	QLabel *labelserver, *labelname;
	QPushButton *ok;

	labelserver = new QLabel(i18n("GGZap Reservation server"), this);
	labelname = new QLabel(i18n("Player name (12 chars maximum)"), this);

	ok = new QPushButton(i18n("OK"), this);

	m_server = new QLineEdit(this);
	m_username = new QLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(labelserver);
	vbox->add(m_server);
	vbox->add(labelname);
	vbox->add(m_username);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccept()));

	configfile = new QString(getenv("HOME"));
	configfile->append("/.ggz/ggzap.rc");
	configLoad();

	setFixedSize(200, 150);
	setCaption(i18n("GGZap Configuration"));
	show();
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

	conf = new GGZCoreConfio(configfile->latin1(), GGZCoreConfio::readonly);
	m_server->setText(conf->read("Global", "Server", "mindx.dyndns.org"));
	m_username->setText(conf->read("Global", "Username", ""));
	delete conf;
}

void GGZapConfig::configSave()
{
	GGZCoreConfio *conf;

	if(m_username->text().length() > 12) m_username->setText(m_username->text().left(12));
	conf = new GGZCoreConfio(configfile->latin1(), GGZCoreConfio::readwrite | GGZCoreConfio::create);
	conf->write("Global", "Server", m_server->text().latin1());
	conf->write("Global", "Username", m_username->text().latin1());
	conf->commit();
	delete conf;
}

