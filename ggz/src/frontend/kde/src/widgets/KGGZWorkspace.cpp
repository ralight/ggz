#include "KGGZWorkspace.h"

#include "KGGZCommon.h"

#include <qlayout.h>

KGGZWorkspace::KGGZWorkspace(QWidget *parent, char *name)
:  QWidget(parent, name)
{
	QSplitter *hsbox;
	QHBoxLayout *hbox;
	QWidget *widget;

	m_vsbox = new QSplitter(this);
	m_vsbox->setOrientation(QSplitter::Vertical);

	widget = new QWidget(m_vsbox);

	m_logo = new QFrame(widget);
	m_logo->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	m_logo->setBackgroundColor(QColor(255.0, 0.0, 0.0));
	m_logo->setMinimumSize(64, 64);

	m_tables = new KGGZTables(widget, "workspace_tables");
	m_tables->setMinimumHeight(64);

	hsbox = new QSplitter(QSplitter::Horizontal, m_vsbox);

	m_userlist = new KGGZUsers(hsbox, "workspace_userlist");
	m_userlist->setMinimumWidth(1);
	m_chat = new KGGZChat(hsbox, "workspace_chat");

	hbox = new QHBoxLayout(widget, 5);
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
	KGGZDEBUG("resize workspace to: %i/%i!\n", e->size().width(), e->size().height());
	m_vsbox->resize(e->size().width(), e->size().height());
	show();
}

QFrame *KGGZWorkspace::widgetLogo()
{
	return m_logo;
}
