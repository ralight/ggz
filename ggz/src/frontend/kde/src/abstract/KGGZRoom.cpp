#include "KGGZRoom.h"

#include "KGGZCommon.h"

KGGZRoom* KGGZRoom::m_instance = NULL;

KGGZRoom::KGGZRoom(GGZRoom *room)
: GGZCoreRoom(room)
{
	KGGZDEBUGF("KGGZRoom::KGGZRoom()\n");
	/*addHook(playerlist, hookPlayerlist);
	addHook(tablelist, hookTablelist);
	KGGZDEBUG("chat hooks\n");
	addHook(chatnormal, hookChatNormal);
	addHook(chatannounce, hookChatAnnounce);
	addHook(chatprivate, hookChatPrivate);
	addHook(chatbeep, hookChatBeep);
	KGGZDEBUG("transition hooks\n");
	addHook(enter, hookEnter);
	addHook(leave, hookLeave);
	KGGZDEBUG("table hooks\n");
	addHook(tableupdate, hookTableUpdate);
	addHook(tablelaunched, hookTableLaunched);
	addHook(tablelaunchfail, hookTableLaunchFail);
	addHook(tablejoined, hookTableJoined);
	addHook(tablejoinfail, hookTableJoinFail);
	addHook(tableleft, hookTableLeft);
	addHook(tableleavefail, hookTableLeaveFail);
	addHook(tabledata, hookTableData);*/
	m_instance = this;
	KGGZDEBUGF("KGGZRoom::KGGZRoom() ready\n");
}

KGGZRoom::~KGGZRoom()
{
	KGGZDEBUGF("KGGZRoom::~KGGZRoom()\n");
	/*removeHook(playerlist, hookPlayerlist);
	removeHook(tablelist, hookTablelist);
	removeHook(chatnormal, hookChatNormal);
	removeHook(chatannounce, hookChatAnnounce);
	removeHook(chatprivate, hookChatPrivate);
	removeHook(chatbeep, hookChatBeep);
	removeHook(enter, hookEnter);
	removeHook(leave, hookLeave);
	removeHook(tableupdate, hookTableUpdate);
	removeHook(tablelaunched, hookTableLaunched);
	removeHook(tablelaunchfail, hookTableLaunchFail);
	removeHook(tablejoined, hookTableJoined);
	removeHook(tablejoinfail, hookTableJoinFail);
	removeHook(tableleft, hookTableLeft);
	removeHook(tableleavefail, hookTableLeaveFail);
	removeHook(tabledata, hookTableData);*/
	m_instance = NULL;
	KGGZDEBUGF("KGGZRoom::~KGGZRoom() ready\n");
}

KGGZRoom* KGGZRoom::instance()
{
	return m_instance;
}

GGZHookReturn KGGZRoom::hookPlayerlist(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookPlayerlist()\n");
}

GGZHookReturn KGGZRoom::hookTablelist(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookTablelist()\n");
}

GGZHookReturn KGGZRoom::hookChatNormal(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookChatNormal()\n");
}

GGZHookReturn KGGZRoom::hookChatAnnounce(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookChatAnnounce()\n");
}

GGZHookReturn KGGZRoom::hookChatPrivate(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookChatPrivate()\n");
}

GGZHookReturn KGGZRoom::hookChatBeep(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookChatBeep()\n");
}

GGZHookReturn KGGZRoom::hookEnter(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookEnter()\n");
}

GGZHookReturn KGGZRoom::hookLeave(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookLeave()\n");
}

GGZHookReturn KGGZRoom::hookTableUpdate(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookTableUpdate()\n");
}

GGZHookReturn KGGZRoom::hookTableLaunched(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookTableLaunched()\n");
}

GGZHookReturn KGGZRoom::hookTableLaunchFail(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookTableLaunchFail()\n");
}

GGZHookReturn KGGZRoom::hookTableJoined(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookTableJoined()\n");
}

GGZHookReturn KGGZRoom::hookTableJoinFail(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookTableJoinFail()\n");
}

GGZHookReturn KGGZRoom::hookTableLeft(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookTableLeft()\n");
}

GGZHookReturn KGGZRoom::hookTableLeaveFail(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookTableLeaveFail()\n");
}

GGZHookReturn KGGZRoom::hookTableData(GGZCoreRoomEvent id, void *event_data, void *user_data)
{
	KGGZDEBUGF("KGGZRoom::hookTableData()\n");
}

