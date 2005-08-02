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
// KGGZBrowser: Embedded HTML browser component for help, games, updates and news. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#include "KGGZBrowser.h"

// KGGZ includes
#include "KGGZCommon.h"

// KDE includes
#include <khtmlview.h>
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>

KGGZBrowser::KGGZBrowser(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	//KHTMLView *view;
	QPushButton *home;

	//setEraseColor(QColor(255, 0, 255));

	m_part = new KHTMLPart(this);
	//m_part->view()->resize(500, 400);
	//part->setJavaEnabled(ggzcore_conf_read_int("KGGZ-Settings", "Java", 0));

	home = new QPushButton(i18n("Home"), this);

	m_edit = new QLineEdit(this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(m_part->view());
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(home);
	hbox->add(m_edit);

	connect(m_part->browserExtension(), SIGNAL(openURLRequest(const KURL&, const KParts::URLArgs&)),
		SLOT(slotRequest(const KURL&, const KParts::URLArgs&)));
	connect(m_edit, SIGNAL(returnPressed()), SLOT(slotRequestExplicit()));
	connect(home, SIGNAL(clicked()), SLOT(slotRequestHome()));

	slotRequestHome();
}

KGGZBrowser::~KGGZBrowser()
{
}

void KGGZBrowser::slotRequest(const KURL& url, const KParts::URLArgs& args)
{
	m_edit->setText(url.url());
	m_part->openURL(url.url());
}

void KGGZBrowser::slotRequestExplicit()
{
	m_part->openURL(m_edit->text());
}

void KGGZBrowser::slotRequestHome()
{
	m_edit->setText(KGGZ_DIRECTORY "/help/index.html");
	slotRequestExplicit();
}
