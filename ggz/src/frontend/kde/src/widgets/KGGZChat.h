/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4           //
// Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net //
// The MindX Open Source Project - http://mindx.sourceforge.net            //
// Published under GNU GPL conditions - view COPYING for details           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_CHAT_H
#define KGGZ_CHAT_H

// KDE includes
#include <ktextbrowser.h>

// Qt includes
#include <qlineedit.h>
#include <qstring.h>

// System includes
#include <sys/time.h>

// Structure for client-side lag measurement
struct Lag
{
	long lagid;
	struct timeval lagtime;
};

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
			RECEIVE_INFO
		};

		KGGZChat(QWidget *parent, char *name);
		~KGGZChat();
		void init();
		void receive(const char *player, const char *message, ReceiveMode mode);
		void beep();

		// whether or not display a players listing in the chat
		int m_listusers;

	signals:
		void signalChat(char *text);

	protected slots:
		void slotSend();

	private:
		void logChat(QString text);
		long setLag(long lagid);
		long getLag(long lagid);
		long randomLag();
		void checkLag(const char *text);
		int separator(const char *c);
		char *plaintext(const char *text);
		void parse(char *text);

		KTextBrowser *output;
		QLineEdit *input;
		Lag lag[10];
		int xlag;
};

#endif
