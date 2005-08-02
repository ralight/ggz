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
// KGGZChatLine: Replace the simple line input with one which autocompletes names. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZChatLine.h"

// KGGZ includes
#include "KGGZCommon.h"

KGGZChatLine::KGGZChatLine(QWidget *parent, const char *name)
: KLineEdit(parent, name)
{
	current = m_history.begin();

	m_history << QString::null;

	setCompletionMode(KGlobalSettings::CompletionAuto);
}

KGGZChatLine::~KGGZChatLine()
{
	removeAll();
}

void KGGZChatLine::removeAll()
{
	m_history.clear();
	completionObject()->clear();
}

void KGGZChatLine::addPlayer(QString name)
{
	completionObject()->addItem(name);
}

void KGGZChatLine::removePlayer(QString name)
{
	completionObject()->removeItem(name);
}

void KGGZChatLine::keyPressEvent(QKeyEvent *e)
{
	QStringList list;
	QStringList::iterator it;

	if(e->key() == Qt::Key_Up)
	{
		if(current != m_history.begin())
			current--;
		else current = m_history.end();
		setText((*current));
		return;
	}

	if(e->key() == Qt::Key_Down)
	{
		if(current != m_history.end())
			current++;
		else current = m_history.begin();
		setText((*current));
		return;
	}

	if((e->key() == Qt::Key_Enter)
	|| (e->key() == Qt::Key_Return))
	{
		m_history << text();
		current = m_history.end();
		m_complete = QString::null;
	}

	KLineEdit::keyPressEvent(e);

	// TODO: autocompletion of words within text
	/*list = QStringList::split(QChar(' '), text());
	it = list.end();
	it--;
	m_complete = (*it);*/
	m_complete = text();
}

void KGGZChatLine::focusOutEvent(QFocusEvent *e)
{
	QString ret;

	if(!m_complete.isNull())
	{
		setFocus();
		ret = completionObject()->makeCompletion(m_complete);
		if(!ret.isNull())
		{
			makeCompletion(text());
			setText(text() + ": ");
		}
		else setText(text());
	}
}

