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
#include "shadowcontainer.h"

// KDE includes
#include <klistview.h>
#include <kcombobox.h>
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qdatetime.h>
#include <qpixmap.h>
#include <qlabel.h>

// Configuration files
#include "config.h"

ShadowContainer::ShadowContainer(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QLabel *label;

	view = new KListView(this);
	view->setSorting(0, TRUE);
	view->addColumn(i18n("Time"));
	view->addColumn(i18n("Event"));
	view->addColumn(i18n("Data"));

	label = new QLabel(i18n("Active client:"), this);

	combo = new KComboBox(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label);
	vbox->add(combo);
	vbox->add(view);

	connect(combo, SIGNAL(activated(int)), SIGNAL(signalActivated(int)));
}

ShadowContainer::~ShadowContainer()
{
}

void ShadowContainer::slotIncoming(const QString& data)
{
	addEntry(data, KGGZ_DIRECTORY "/shadowbridge/incoming.png");
}

void ShadowContainer::slotOutgoing(const QString& data)
{
	addEntry(data, KGGZ_DIRECTORY "/shadowbridge/outgoing.png");
}

void ShadowContainer::slotAdmin(const QString& data)
{
	addEntry(data, KGGZ_DIRECTORY "/shadowbridge/admin.png");
	combo->insertItem(data);
}

void ShadowContainer::addEntry(const QString &data, const char *pixmap)
{
	KListViewItem *tmp;

	tmp = new KListViewItem(view);
	tmp->setText(0, QTime::currentTime().toString());
	tmp->setPixmap(1, QPixmap(pixmap));
	tmp->setText(2, data);
}

