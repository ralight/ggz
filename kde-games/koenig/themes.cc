// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
// Copyright (C) 2001 - 2004 Josef Spillner <josef@ggzgamingzone.org>
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

#include "themes.h"

#include <klocale.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kconfig.h>
#include <kapplication.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qdir.h>

Themes::Themes(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QLabel *heading;
	QPushButton *ok;

	heading = new QLabel(i18n("Available chess themes:"), this);

	ok = new QPushButton(i18n("Select"), this);

	m_themes = new QComboBox(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(heading);
	vbox->add(m_themes);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotTheme()));

	scan();

	kapp->config()->setGroup("Theme");
	QString theme = kapp->config()->readEntry("Theme");
	QMap<QString, QString>::Iterator it;
	for(it = m_thememap.begin(); it != m_thememap.end(); it++)
		if(it.key() == theme) m_themes->setCurrentText(it.data());

	setCaption(i18n("Theme Selector"));
	show();
}

Themes::~Themes()
{
}

void Themes::scanDir(QString directory)
{
	QString theme;

	QString basedir = directory;
	QDir dir(basedir);
	QStringList s = dir.entryList(QDir::Dirs);
	for(QStringList::iterator it = s.begin(); it != s.end(); it++)
	{
		if(((*it) == ".") || ((*it) == "..")) continue;

		KSimpleConfig conf(basedir + "/" + (*it) + "/theme.conf");
		if(conf.hasGroup("General"))
		{
			conf.setGroup("General");
			QString name = conf.readEntry("Name");
			theme = i18n("%1 (Knights theme)").arg(name);
		}
		else theme = i18n("%1 (GGZ)").arg(*it);

		m_thememap[theme] = basedir + "/" + (*it);
		m_themes->insertItem(theme);
	}
}

void Themes::scan()
{
	KStandardDirs d;

	QStringList basedirs = d.findDirs("data", "koenig");
	for(QStringList::iterator it = basedirs.begin(); it != basedirs.end(); it++)
		scanDir(*it);
}

void Themes::slotTheme()
{
	kapp->config()->setGroup("Theme");
	kapp->config()->writeEntry("Theme", m_thememap[m_themes->currentText()]);
	kapp->config()->sync();

	emit signalTheme(m_thememap[m_themes->currentText()]);
	close();
}

