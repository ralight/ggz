#include "kggz_state.h"

#include <qlayout.h>
#include <qcolor.h>

#include "kggz_chatwidget.h"

#include <iostream.h>

KGGZ_State::KGGZ_State(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QHBoxLayout *hbox;
	QWidget *statebox;

	hbox = new QHBoxLayout(this, 5);

	label = new QLabel("", this);
	statebox = new QWidget(this);
	statebox->setFixedSize(20, 20);
	statebox->setBackgroundColor(QColor(255, 255, 0));

	hbox->add(statebox);
	hbox->add(label);

	show();
}

KGGZ_State::~KGGZ_State()
{
}

void KGGZ_State::showState(char *state)
{
	label->setText(state);
}

void KGGZ_State::registerStates()
{
	ggzcore_state_add_callback(GGZ_STATE_OFFLINE, changedState);
	ggzcore_state_add_callback(GGZ_STATE_CONNECTING, changedState);
	ggzcore_state_add_callback(GGZ_STATE_ONLINE, changedState);
	ggzcore_state_add_callback(GGZ_STATE_LOGGING_IN, changedState);
	ggzcore_state_add_callback(GGZ_STATE_LOGGED_IN, changedState);
	ggzcore_state_add_callback(GGZ_STATE_BETWEEN_ROOMS, changedState);
	ggzcore_state_add_callback(GGZ_STATE_ENTERING_ROOM, changedState);
	ggzcore_state_add_callback(GGZ_STATE_IN_ROOM, changedState);
	ggzcore_state_add_callback(GGZ_STATE_JOINING_TABLE, changedState);
	ggzcore_state_add_callback(GGZ_STATE_AT_TABLE, changedState);
	ggzcore_state_add_callback(GGZ_STATE_LEAVING_TABLE, changedState);
	ggzcore_state_add_callback(GGZ_STATE_LOGGING_OUT, changedState);
}

char *KGGZ_State::getState(GGZStateID state)
{
	switch (state)
	{
		case GGZ_STATE_OFFLINE:
			return "Offline";
			break;
		case GGZ_STATE_CONNECTING:
			return "Connecting";
			break;
		case GGZ_STATE_ONLINE:
			return "Online";
			break;
		case GGZ_STATE_LOGGING_IN:
			return "Logging In";
			break;
		case GGZ_STATE_LOGGED_IN:
			return "Logged In";
			break;
		case GGZ_STATE_BETWEEN_ROOMS:
		case GGZ_STATE_ENTERING_ROOM:
			return "--> Room";
			break;
		case GGZ_STATE_IN_ROOM:
			return "Chatting";
			break;
		case GGZ_STATE_JOINING_TABLE:
			return "--> Table";
			break;
		case GGZ_STATE_AT_TABLE:
			return "Playing";
			break;
		case GGZ_STATE_LEAVING_TABLE:
			return "<-- Table";
			break;
		case GGZ_STATE_LOGGING_OUT:
			return "Logging Out";
			break;
		default:
			return "**none**";
	}
}

void KGGZ_State::changedState(GGZStateID id, void *state_data, void *user_data)
{
	KGGZ_Chatwidget::adminreceive(getState(id));
	cout << "KGGZ_State: " << getState(id) << endl;
}