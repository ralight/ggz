#include "kggz_chat.h"
#include <qlayout.h>

KGGZ_Chat::KGGZ_Chat(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;
	QHBoxLayout *hbox1;

	tables = new KGGZ_Tables(this, "tables");
	tables->setFixedHeight(100);

	chatwidget = new KGGZ_Chatwidget(this, "chatwidget");

	userlist = new KGGZ_Users(this, "userlist");
	userlist->setFixedWidth(200);

	vbox1 = new QVBoxLayout(this, 0);
	vbox1->add(tables);

	hbox1 = new QHBoxLayout(vbox1, 0);
	hbox1->add(userlist);
	hbox1->add(chatwidget);
}

KGGZ_Chat::~KGGZ_Chat()
{
}
