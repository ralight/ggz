/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.4           //
// Copyright (C) 2000, 2001 Josef Spillner - dr_maux@users.sourceforge.net //
// The MindX Open Source Project - http://mindx.sourceforge.net            //
// Published under GNU GPL conditions - view COPYING for details           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef KGGZ_MOTD_H
#define KGGZ_MOTD_H

// Qt includes
#include <qwidget.h>
#include <qtextview.h>

// KGGZMotd: displays welcome message
class KGGZMotd : public QWidget
{
Q_OBJECT
	public:
		KGGZMotd(QWidget *parent = NULL, char *name = NULL);
		~KGGZMotd();
		void setSource(void *data);

	private:
		void append(char *text);

		QTextView *edit;
};

#endif
