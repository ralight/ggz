#ifndef KGGZMOD_EVENT_H
#define KGGZMOD_EVENT_H

#include <qmap.h>

namespace KGGZMod
{

class Player;

class Event
{
	friend class ModulePrivate;

	public:
		enum Type
		{
			launch,	/**< Game was launched. */
			server,	/**< Connected to server. */
			self,	/**< Your seat was assigned. */
			seat,	/**< Someone's seat changed. */
			chat,	/**< A chat message was received */
			stats,	/**< Statistics have been received for a player. */
			info	/**< Information about a player has arrived. */
		};

		Event(Type type);

		Type type() const;

		QMap<QString, QString> data;

	protected:
		Player *m_player;

	private:
		Type m_type;
		//Module *m_mod;
		//Player *playerbyname(QString name);
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
		Player *player() const;
};

class InfoEvent : public Event
{
	public:
		InfoEvent(const Event& event);
		Player *player() const;
};

}

#endif

