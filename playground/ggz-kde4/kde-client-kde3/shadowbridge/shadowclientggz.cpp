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
#include "shadowclientggz.h"

// KDE includes
#include <kcombobox.h>
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

// GGZ includes
#include "GGZCoreConfio.h"

// System includes
#include <stdlib.h>
#include <string.h>

ShadowClientGGZ::ShadowClientGGZ(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QLabel *label;
	QPushButton *ok, *cancel;
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;

	m_activated = 0;

	ok = new QPushButton(i18n("OK"), this);
	cancel = new QPushButton(i18n("Cancel"), this);

	label = new QLabel(i18n("Please select a GGZ game client:"), this);

	combo = new KComboBox(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label);
	vbox->add(combo);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(ok);
	hbox->add(cancel);

	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	connect(ok, SIGNAL(clicked()), SLOT(slotPressed()));
	connect(combo, SIGNAL(activated(int)), SLOT(slotActivated(int)));

	init();
}

ShadowClientGGZ::~ShadowClientGGZ()
{
}

void ShadowClientGGZ::init()
{
	GGZCoreConfio *conf;
	char **list, **felist;
	int count, fecount, ret;
	char *fe;

	conf = new GGZCoreConfio("/usr/local/etc/ggz.modules", GGZCoreConfio::readonly);
	ret = conf->read("Games", "*Engines*", &count, &list);	
	if(ret == -1) return;

	if(list)
	{
		for(int i = 0; i < count; i++)
		{
			ret = conf->read("Games", list[i], &fecount, &felist);
			if(ret != -1)
			{
				if(felist)
				{
					for(int j = 0; j < fecount; j++)
					{
						fe = conf->read(felist[j], "Frontend", "unknown");
						combo->insertItem(QString("%1 (%2)").arg(list[i]).arg(fe));
						fe = conf->read(felist[j], "CommandLine", "");
						cmdlinelist.append(strdup(fe));
						fe = conf->read(felist[j], "Name", list[i]);
						namelist.append(strdup(fe));
						free(felist[j]);
					}
					GGZCoreConfio::free(felist);
				}
			}
			GGZCoreConfio::free(list[i]);
		}
		GGZCoreConfio::free(list);
	}

	delete conf;
}

void ShadowClientGGZ::slotPressed()
{
    emit signalClient(cmdlinelist.at(m_activated), "3", "3");
    close();
}

void ShadowClientGGZ::slotActivated(int index)
{
	m_activated = index;
}

