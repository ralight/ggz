/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
 * Published under GNU GPL conditions.
 */

#ifndef QMETAGUI_H
#define QMETAGUI_H

#include <qwidget.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmultilineedit.h>
#include <qsocket.h>

class QMetaGUI : public QWidget
{
	Q_OBJECT
	public:
		QMetaGUI();
		~QMetaGUI();
	public slots:
		void slotConnected();
		void slotRead();
		void slotURI();
		void slotXML();
	private:
		void doconnection();

		QLineEdit *host, *port;
		QListView *view;
		QMultiLineEdit *raw;
		QSocket *sock;
		int m_type;
};

#endif

