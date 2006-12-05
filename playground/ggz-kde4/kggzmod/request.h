#ifndef KGGZMOD_REQUEST_H
#define KGGZMOD_REQUEST_H

//#include <kggzmod/module.h>

#include <QMap>
#include <QString>

namespace KGGZMod
{

class Request
{
	public:
		enum Type
		{
			state,	/**< Change the state (-> state). */
			stand,	/**< Stand up from seat. */
			sit,	/**< Sit down again if possible (-> seat). */
			boot,	/**< Boot a player from table (-> player). */
			bot,	/**< Let a bot join the game (-> seat). */
			open,	/**< Open up a bot seat (-> seat). */
			chat,	/**< Send a chat message (-> message). */
			info	/**< Request infos about a player (-> seat). */
		};

		Request(Type type);

		Type type() const;

		QMap<QString, QString> data;

	private:
		Type m_type;
};

class StateRequest : public Request
{
	public:
		StateRequest(/*Module::State*/int state);
};

class StandRequest : public Request
{
	public:
		StandRequest();
};

class SitRequest : public Request
{
	public:
		SitRequest(int seat);
};

class BootRequest : public Request
{
	public:
		BootRequest(QString playername);
};

class BotRequest : public Request
{
	public:
		BotRequest(int seat);
};

class OpenRequest : public Request
{
	public:
		OpenRequest(int seat);
};

class ChatRequest : public Request
{
	public:
		ChatRequest(QString message);
};

class InfoRequest : public Request
{
	public:
		InfoRequest(int seat);
};

}

#endif

