#include "KGGZWorkspace.h"

#include "KGGZCommon.h"

#include <qlayout.h>

KGGZWorkspace::KGGZWorkspace(QWidget *parent, char *name)
:  QWidget(parent, name)
{
	QHBoxLayout *hbox;

	m_firstresize = 4;

	setBackgroundColor(QColor(255.0, 255.0, 0.0));

	m_vsbox = new QSplitter(this);
	m_vsbox->setOrientation(QSplitter::Vertical);
	//m_vsbox->setOpaqueResize(TRUE);
	//m_vsbox->resize(500, 400);

	m_widget = new QWidget(m_vsbox);
	m_widget->setMinimumHeight(74);

	m_logo = new KGGZLogo(m_widget);
	m_logo->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	m_logo->setBackgroundColor(QColor(255.0, 0.0, 0.0));
	m_logo->setMinimumSize(64, 64);
	//m_logo->setFixedSize(64, 64);

	m_tables = new KGGZTables(m_widget, "workspace_tables");
	m_tables->setMinimumHeight(64);

	m_hsbox = new QSplitter(QSplitter::Horizontal, m_vsbox);
	//hsbox->setOpaqueResize(TRUE);

	m_userlist = new KGGZUsers(m_hsbox, "workspace_userlist");
	m_userlist->setMinimumWidth(1);

	m_chat = new KGGZChat(m_hsbox, "workspace_chat");

	hbox = new QHBoxLayout(m_widget, 5);
	hbox->add(m_logo);
	hbox->add(m_tables);
}

KGGZWorkspace::~KGGZWorkspace()
{
}

KGGZChat *KGGZWorkspace::widgetChat()
{
	return m_chat;
}

KGGZTables *KGGZWorkspace::widgetTables()
{
	return m_tables;
}

KGGZUsers *KGGZWorkspace::widgetUsers()
{
	return m_userlist;
}

void KGGZWorkspace::resizeEvent(QResizeEvent *e)
{
	QValueList<int> sizelist;

	KGGZDEBUG("resize splitter to: %i/%i!\n", e->size().width(), e->size().height());

	m_vsbox->resize(e->size());

	//if(m_firstresize > 0) m_firstresize--;
	if(m_firstresize)
	{
		KGGZDEBUG("Doing it now! (%i, %i)\n", width(), height());
		m_firstresize = 0;
		sizelist.append(64);
		sizelist.append(height() - 79);
		m_vsbox->setSizes(sizelist);
		sizelist.clear();
		sizelist.append(150);
		sizelist.append(width() - 165);
		m_hsbox->setSizes(sizelist);
	}

	//show();
	//resize(e->size().width(), e->size().height());
	updateGeometry();
}

KGGZLogo *KGGZWorkspace::widgetLogo()
{
	return m_logo;
}
