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
// KGGZChat: display the chat, receive text/events, and send user text via signal. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_CHAT_H
#define KGGZ_CHAT_H

// KGGZ includes
#include "KGGZChatLine.h"

// KDE includes
#include <ktextbrowser.h>

// Qt includes
//#include <qlineedit.h>
#include <qstring.h>

// System includes
#include <sys/time.h>

// KGGZ_Chatwidget: The name says it all
class KGGZChat : public QWidget
{
	Q_OBJECT
	public:
		enum ReceiveMode
		{
			RECEIVE_CHAT,
			RECEIVE_OWN,
			RECEIVE_ADMIN,
			RECEIVE_INFO,
			RECEIVE_PERSONAL,
			RECEIVE_ANNOUNCE,
			RECEIVE_BEEP,
			RECEIVE_ME,
			RECEIVE_TABLE
		};

		KGGZChat(QWidget *parent = NULL, const char *name = NULL);
		~KGGZChat();
		void init();
		void shutdown();
		void receive(const char *player, const char *message, ReceiveMode mode);
		void beep();

		// whether or not display a players listing in the chat
		int m_listusers;

		KGGZChatLine *chatline();

		void setLogging(int log);
		void setSpeech(int speech);
		void setTimestamp(int timestamp);

	signals:
		void signalChat(const char *text, char *player, int mode);

	protected slots:
		void slotSend();

	private:
		void logChat(QString text);
		int separator(const char *c);
		char *plaintext(const char *text);
		QString parse(char *text);

		KTextBrowser *output;
		//QLineEdit *input;
		KGGZChatLine *input;
		int m_log;
		int m_speech;
		int m_timestamp;
};

#endif

