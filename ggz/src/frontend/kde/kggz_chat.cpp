#include "kggz_chat.h"
#include <qlayout.h>

KGGZ_Chat::KGGZ_Chat(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1, *vbox2;
	QHBoxLayout *hbox1;

	tables = new KGGZ_Tables(this, "tables");
	tables->setFixedHeight(100);

	chatwidget = new KGGZ_Chatwidget(this, "chatwidget");

	userlist = new KGGZ_Users(this, "userlist");
	userlist->setFixedWidth(200);

	state = new KGGZ_State(this, "state");
	state->setFixedSize(200, 30);
	state->showState("Connected");

	vbox1 = new QVBoxLayout(this, 0);
	vbox1->add(tables);

	hbox1 = new QHBoxLayout(vbox1, 0);
	vbox2 = new QVBoxLayout(hbox1, 0);
	vbox2->add(userlist);
	vbox2->add(state);
	hbox1->add(chatwidget);
}

KGGZ_Chat::~KGGZ_Chat()
{
}
