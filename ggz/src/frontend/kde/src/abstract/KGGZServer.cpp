#include "KGGZServer.h"
#include "KGGZCommon.h"
#include "KGGZRoom.h"

KGGZServer* KGGZServer::m_instance = NULL;

KGGZServer::KGGZServer()
: GGZCoreServer()
{
	KGGZDEBUGF("KGGZServer::KGGZServer()\n");
	addHook(connected, hookConnected);
	addHook(connectfail, hookConnectFail);
	addHook(negotiated, hookNegotiated);
	addHook(negotiatefail, hookNegotiateFail);
	addHook(loggedin, hookLoggedin);
	addHook(loginfail, hookLoginFail);
	addHook(motdloaded, hookMotdLoaded);
	addHook(roomlist, hookRoomList);
	addHook(typelist, hookTypeList);
	addHook(entered, hookEntered);
	addHook(enterfail, hookEnterFail);
	addHook(loggedout, hookLoggedout);
	addHook(neterror, hookNetError);
	addHook(protoerror, hookProtoError);
	addHook(chatfail, hookChatFail);
	addHook(statechange, hookStateChange);
	m_instance = this;
	KGGZDEBUGF("KGGZServer::KGGZServer() ready\n");
}

KGGZServer::~KGGZServer()
{
	KGGZDEBUGF("KGGZServer::~KGGZServer()\n");
	removeHook(connected, hookConnected);
	removeHook(connectfail, hookConnectFail);
	removeHook(negotiated, hookNegotiated);
	removeHook(negotiatefail, hookNegotiateFail);
	removeHook(loggedin, hookLoggedin);
	removeHook(loginfail, hookLoginFail);
	removeHook(motdloaded, hookMotdLoaded);
	removeHook(roomlist, hookRoomList);
	removeHook(typelist, hookTypeList);
	removeHook(entered, hookEntered);
	removeHook(enterfail, hookEnterFail);
	removeHook(loggedout, hookLoggedout);
	removeHook(neterror, hookNetError);
	removeHook(protoerror, hookProtoError);
	removeHook(chatfail, hookChatFail);
	removeHook(statechange, hookStateChange);
	if(KGGZRoom::instance()) delete KGGZRoom::instance();
	m_instance = NULL;
	KGGZDEBUGF("KGGZServer::~KGGZServer() ready\n");
}

int KGGZServer::countRoomsForce()
{
	int result;

	result = countRooms();

	if(!result)
	{
		listRooms(-1, 0);
		while(!countRooms()) dataRead();
		result = countRooms();
	}

	return result;
}

GGZHookReturn KGGZServer::hookConnected(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookConnected()\n");
}

GGZHookReturn KGGZServer::hookConnectFail(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookConnectFail()\n");
}

GGZHookReturn KGGZServer::hookNegotiated(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookNegotiated()\n");
}

GGZHookReturn KGGZServer::hookNegotiateFail(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookNegotateFail()\n");
}

GGZHookReturn KGGZServer::hookLoggedin(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookLoggedin()\n");
}

GGZHookReturn KGGZServer::hookLoginFail(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookLoginFail()\n");
}

GGZHookReturn KGGZServer::hookMotdLoaded(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookMotdLoaded()\n");
}

GGZHookReturn KGGZServer::hookRoomList(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookRoomList()\n");
}

GGZHookReturn KGGZServer::hookTypeList(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookTypeList()\n");
}

GGZHookReturn KGGZServer::hookEntered(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	GGZCoreRoom* ggzcoreroom;
	KGGZRoom* kggzroom;

	KGGZDEBUGF("KGGZServer::hookEntered()\n");

	if(!KGGZServer::instance())
	{
		KGGZDEBUG("Critical: No server found!\n");
		return GGZ_HOOK_ERROR;
	}

	if(KGGZRoom::instance()) delete KGGZRoom::instance();
	ggzcoreroom = KGGZServer::instance()->room();
	kggzroom = new KGGZRoom(ggzcoreroom->room());
	return GGZ_HOOK_OK;
}

GGZHookReturn KGGZServer::hookEnterFail(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookEnterFail()\n");
}

GGZHookReturn KGGZServer::hookLoggedout(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookLoggedout()\n");

	if(KGGZRoom::instance()) delete KGGZRoom::instance();
}

GGZHookReturn KGGZServer::hookNetError(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookNetError()\n");
}

GGZHookReturn KGGZServer::hookProtoError(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookProtoError()\n");
}

GGZHookReturn KGGZServer::hookChatFail(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZServer::hookChatFail()\n");
}

GGZHookReturn KGGZServer::hookStateChange(GGZCoreServerEvent id, void *event_data, void *user_data)
{
	GGZStateID stateid;
	char *statestr;

	KGGZDEBUGF("KGGZServer::hookStateChange()\n");

	/*stateid = KGGZServer::instance()->state();
	statestr = KGGZCommon::state(stateid);*/

	/*KGGZDEBUG("State is: %s\n", statestr);*/
}

KGGZServer* KGGZServer::instance()
{
	return m_instance;
}
