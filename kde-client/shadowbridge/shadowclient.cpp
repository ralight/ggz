// ShadowBridge - Game developer tool to visualize network protocols
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

// ShadowBridge includes
#include "shadowclient.h"

// KDE includes
#include <klineedit.h>
#include <qpushbutton.h>
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qlabel.h>

ShadowClient::ShadowClient(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *label_exec, *label_fdin, *label_fdout;
	QPushButton *ok, *cancel;

	label_exec = new QLabel(i18n("Application command line:"), this);
	label_fdin = new QLabel(i18n("Inbound socket:"), this);
	label_fdout = new QLabel(i18n("Outbound socket:"), this);

	edit_exec = new KLineEdit(this);
	edit_fdin = new KLineEdit("3", this);
	edit_fdout = new KLineEdit("3", this);

	ok = new QPushButton(i18n("OK"), this);
	cancel = new QPushButton(i18n("Cancel"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label_exec);
	vbox->add(edit_exec);
	vbox->add(label_fdin);
	vbox->add(edit_fdin);
	vbox->add(label_fdout);
	vbox->add(edit_fdout);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(ok);
	hbox->add(cancel);

	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	connect(ok, SIGNAL(clicked()), SLOT(slotPressed()));

	show();
}

ShadowClient::~ShadowClient()
{
}

void ShadowClient::slotPressed()
{
	emit signalClient(edit_exec->text(), edit_fdin->text(), edit_fdout->text());
	close();
}

