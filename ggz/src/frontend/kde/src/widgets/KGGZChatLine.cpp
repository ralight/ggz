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
// KGGZChatLine: Replace the simple line input with one which autocompletes names. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZChatLine.h"

// KGGZ includes
#include "KGGZCommon.h"

KGGZChatLine::KGGZChatLine(QWidget *parent, const char *name)
: QLineEdit(parent, name)
{
}

KGGZChatLine::~KGGZChatLine()
{
	removeAll();
}

void KGGZChatLine::removeAll()
{
	m_list.clear();
}

void KGGZChatLine::addPlayer(char *name)
{
	m_list.append(QString(name));
}

void KGGZChatLine::removePlayer(char *name)
{
	m_list.remove(QString(name));
}

void KGGZChatLine::keyPressEvent(QKeyEvent *e)
{
	int mark;
	int pos;

	// This is heavy: reimplement all special key just to get autocompletion...
	//cout << e->ascii() << " - " << e->key() << endl;
	KGGZDEBUG("Key: %i, Ascii: %i\n", e->key(), e->ascii());

	mark = FALSE;
	if(e->state() & Qt::ShiftButton) mark = TRUE;

	if(e->state() & Qt::ControlButton)
	{
		switch(e->key())
		{
			case Qt::Key_C:
				copy();
				return;
			case Qt::Key_V:
				paste();
				return;
			case Qt::Key_X:
				cut();
				return;
			default:
				return;
		}
	}

	switch(e->key())
	{
		case Qt::Key_Tab:
			//cout << "Search names beginning with " << text().left(cursorPosition()) << endl;
			KGGZDEBUG("Search names beginning with %s\n", text().left(cursorPosition()).latin1());
			autocomplete(text().left(cursorPosition()));
			break;
		case Qt::Key_Left:
			cursorLeft(mark);
			break;
		case Qt::Key_Right:
			cursorRight(mark);
			break;
		case Qt::Key_Delete:
			del();
			break;
		case Qt::Key_Backspace:
			backspace();
			break;
		case Qt::Key_Home:
			home(mark);
			break;
		case Qt::Key_End:
			end(mark);
			break;
		case Key_Return:
		case Key_Enter:
			emit returnPressed();
			break;
		case Qt::Key_Insert:
		case Qt::Key_Escape:
		case Qt::Key_Shift:
		case Qt::Key_Meta:
		case Qt::Key_Alt:
		case Qt::Key_Control:
		case Qt::Key_unknown:
		case Qt::Key_CapsLock:
		case Qt::Key_ScrollLock:
		case Qt::Key_NumLock:
		case Qt::Key_Up:
		case Qt::Key_Down:
			// nothing
			break;
		default:
			pos = cursorPosition();
			setText(text().insert(pos, e->ascii()));
			setCursorPosition(pos + 1);
			//setText(text() + e->key());
	}
}

void KGGZChatLine::autocomplete(QString pattern)
{
	QStringList::Iterator it;
	QString tmp;
	int pos;
	int count;

	pos = 0;
	count = 0;

	for(int i = 0; i < cursorPosition(); i++)
		if(text().at(i).latin1() == ' ') pos = i + 1;

	pattern = pattern.right(cursorPosition() - pos);

	//cout << "Position: " << pos << endl;
	//cout << "Pattern: " << pattern.latin1() << endl;
	KGGZDEBUG("Position: %i\n", pos);
	KGGZDEBUG("Pattern: %s\n", pattern.latin1());

	if((pattern.isNull()) || (pattern.isEmpty())) return;

	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it).findRev(pattern, 0, FALSE) != -1)
		{
			//cout << "Found: " << (*it).latin1() << endl;
			KGGZDEBUG("Found: %s\n", (*it).latin1());
			tmp = (*it).latin1();
			count++;
		}
	}

	if(count == 1)
	{
		//cout << " => replace!" << endl;
		if(pos == 0) tmp.append(":");
		tmp.append(" ");
		setText(text().replace(pos, pattern.length(), tmp));
	}
}

void KGGZChatLine::focusOutEvent(QFocusEvent *e)
{
	setFocus();
	autocomplete(text().left(cursorPosition()));
}

