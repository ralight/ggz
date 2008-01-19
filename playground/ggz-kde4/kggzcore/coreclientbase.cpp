/*
    This file is part of the kggzdcore library.
    Copyright (c) 2008 Josef Spillner <josef@ggzgamingzone.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <kggzcore/coreclientbase.h>

using namespace KGGZCore;

CoreClientBase::CoreClientBase(QObject *parent)
: QObject(parent)
{
	m_server = NULL;

	init();
}

CoreClientBase::~CoreClientBase()
{
	if(m_server)
		ggzcore_server_free(m_server);

	ggzcore_destroy();
}

void CoreClientBase::init()
{
	GGZOptions options;

	options.flags = (GGZOptionFlags)(GGZ_OPT_PARSER | GGZ_OPT_MODULES);

	int ret = ggzcore_init(options);
	if(ret == -1)
	{
		emit signalBaseError();
		return;
	}

	m_server = ggzcore_server_new();

	ggzcore_server_add_event_hook_full(m_server, GGZ_CONNECTED, &CoreClientBase::cb_server, this);
}

void CoreClientBase::setConnectionInfo(const char *host, int port, const char *username, const char *password, const char *email, GGZLoginType mode, int tls)
{
	ggzcore_server_set_hostinfo(m_server, host, port, tls);
	ggzcore_server_set_logininfo(m_server, mode, username, password, email);
}

void CoreClientBase::startConnection()
{
	int ret = ggzcore_server_connect(m_server);
	if(ret == -1)
	{
		emit signalBaseError();
		return;
	}
}

void CoreClientBase::callback_server(unsigned int id, const void *event_data) const
{
	qDebug("cb_server!");

	int errorcode = E_OK;

	if((id == GGZ_LOGIN_FAIL)
	|| (id == GGZ_ENTER_FAIL)
	|| (id == GGZ_PROTOCOL_ERROR)
	|| (id == GGZ_CHAT_FAIL))
	{
		errorcode = ((const GGZErrorEventData*)event_data)->status;
	}

	emit signalBaseServer(id, errorcode);
}

GGZHookReturn CoreClientBase::cb_server(unsigned int id, const void *event_data, const void *user_data)
{
	qDebug("cb_server-static!");

	static_cast<const CoreClientBase*>(user_data)->callback_server(id, event_data);

	return GGZ_HOOK_OK;
}

