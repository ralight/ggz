#include "network.h"

Network *Network::me;
GGZMod *Network::mod;

Network::Network()
: QObject()
{
	me = this;
}

Network::~Network()
{
}

void Network::connect()
{
	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &handle);
	ggzmod_connect(mod);
	m_cfd = ggzmod_get_fd(mod);
}

void Network::shutdown()
{
	ggzmod_disconnect(mod);
	ggzmod_free(mod);
}

void Network::handle(GGZMod *mod, GGZModEvent e, const void *data)
{
	me->m_fd = *(const int*)data;
	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	emit me->signalData();
}

void Network::slotDispatch()
{
	ggzmod_dispatch(mod);
}

int Network::fd()
{
	return m_fd;
}

int Network::cfd()
{
	return m_cfd;
}

