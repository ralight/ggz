#ifndef KGGZ_ROOM_H
#define KGGZ_ROOM_H

#include <GGZCoreRoom.h>

// This _is_ crap !!!!!
#define GGZCoreRoomEvent unsigned int

class KGGZRoom : public GGZCoreRoom
{
	public:
		KGGZRoom(GGZRoom* room);
		~KGGZRoom();
		static KGGZRoom* instance();

	protected:
		static GGZHookReturn hookPlayerlist(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookTablelist(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookChatNormal(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookChatAnnounce(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookChatPrivate(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookChatBeep(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookEnter(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookLeave(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookTableUpdate(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookTableLaunched(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookTableLaunchFail(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookTableJoined(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookTableJoinFail(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookTableLeft(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookTableLeaveFail(GGZCoreRoomEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookTableData(GGZCoreRoomEvent id, void *event_data, void *user_data);

	private:
		static KGGZRoom* m_instance;
};

#endif
