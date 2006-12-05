#ifndef KGGZMOD_EVENT_H
#define KGGZMOD_EVENT_H

#include <QMap>
#include <QString>

namespace KGGZMod
{

class Player;

class Event
{
	public:
		enum Type
		{
			launch,		/**< Game was launched. */
			server,		/**< Connected to server (-> fd). */
			self,		/**< Your seat was assigned (-> player). */
			seat,		/**< Someone's seat changed (-> player). */
			chat,		/**< A chat message was received (-> player, message) */
			stats,		/**< Statistics have been received for all player. */
			info		/**< Information about a player has arrived (-> player). */
		};

		Event(Type type);

		Type type() const;

		QMap<QString, QString> data;

	private:
		Type m_type;
};

class LaunchEvent : public Event
{
	public:
		LaunchEvent(const Event& event);
};

class ServerEvent : public Event
{
	public:
		ServerEvent(const Event& event);
		int fd() const;
};

class SelfEvent : public Event
{
	public:
		SelfEvent(const Event& event);
		Player *self() const;
};

class SeatEvent : public Event
{
	public:
		SeatEvent(const Event& event);
		Player *player() const;
};

class ChatEvent : public Event
{
	public:
		ChatEvent(const Event& event);
		Player *player() const;
		QString message() const;
};

class StatsEvent : public Event
{
	public:
		StatsEvent(const Event& event);
};

class InfoEvent : public Event
{
	public:
		InfoEvent(const Event& event);
		Player *player() const;
};

}

#endif

