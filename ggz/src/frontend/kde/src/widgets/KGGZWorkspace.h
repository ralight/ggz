#ifndef KGGZ_WORKSPACE_H
#define KGGZ_WORKSPACE_H

#include <qwidget.h>
#include <qevent.h>
#include <qsplitter.h>
#include <qframe.h>

#include "KGGZChat.h"
#include "KGGZTables.h"
#include "KGGZUsers.h"
#include "KGGZLogo.h"

class KGGZWorkspace : public QWidget
{
	Q_OBJECT
	public:
		KGGZWorkspace(QWidget *parent, char *name);
		~KGGZWorkspace();
		KGGZChat *widgetChat();
		KGGZTables *widgetTables();
		KGGZUsers *widgetUsers();
		KGGZLogo *widgetLogo();

	protected:
		void resizeEvent(QResizeEvent *e);

	private:
		KGGZChat *m_chat;
		KGGZTables *m_tables;
		KGGZUsers *m_userlist;
		QSplitter *m_vsbox;
		KGGZLogo *m_logo;
		int m_firstresize;
		QSplitter *m_hsbox;
		QWidget *m_widget;
};

#endif
