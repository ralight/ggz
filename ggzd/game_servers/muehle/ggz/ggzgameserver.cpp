#include "ggzgameserver.h"
#include <iostream>

GGZGameServer* self;
GGZdMod* ggzdmod;

GGZGameServer::GGZGameServer () {
	self = this;
	ggzdmod = ggzdmod_new ( GGZDMOD_GAME );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_STATE, &handle_state );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_JOIN,  &handle_join  );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_LEAVE, &handle_leave );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_PLAYER_DATA, &handle_data );
	ggzdmod_set_handler ( ggzdmod, GGZDMOD_EVENT_ERROR, &handle_error );
}

GGZGameServer::~GGZGameServer () {
	(void) ggzdmod_disconnect ( ggzdmod );
	ggzdmod_free ( ggzdmod );
}

void GGZGameServer::connect () {
	int ret = ggzdmod_connect ( ggzdmod );
	if ( ret < 0 ) {
		std::cout << "GGZGameServer: Error: Couldn't connect" << std::endl;
		return;
	}
	(void) ggzdmod_loop ( ggzdmod );
}

void GGZGameServer::stateEvent () {
}

void GGZGameServer::joinEvent () {
}

void GGZGameServer::leaveEvent () {
}

void GGZGameServer::dataEvent () {
}

void GGZGameServer::errorEvent () {
}

void GGZGameServer::handle_state ( GGZdMod* ggzdmod, GGZdModEvent event, void* data ) {
	std::cout << "GGZGameServer: stateEvent" << std::endl;
	self->stateEvent ();
}

void GGZGameServer::handle_join ( GGZdMod* ggzdmod, GGZdModEvent event, void* data ) {
	std::cout << "GGZGameServer: joinEvent" << std::endl;
	self->joinEvent ();
}

void GGZGameServer::handle_leave ( GGZdMod* ggzdmod, GGZdModEvent event, void* data ) {
	std::cout << "GGZGameServer: leaveEvent" << std::endl;
	self->leaveEvent ();
}

void GGZGameServer::handle_data ( GGZdMod* ggzdmod, GGZdModEvent event, void* data ) {
	std::cout << "GGZGameServer: dataEvent" << std::endl;
	self->dataEvent ();
}

void GGZGameServer::handle_error ( GGZdMod* ggzdmod, GGZdModEvent event, void* data ) {
	std::cout << "GGZGameServer: errorEvent" << std::endl;
	self->errorEvent ();
}

