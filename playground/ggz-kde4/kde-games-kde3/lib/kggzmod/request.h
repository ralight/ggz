#ifndef KGGZMOD_REQUEST_H
#define KGGZMOD_REQUEST_H

#include <qmap.h>

namespace KGGZMod
{

/**
 * @short Requests to the GGZ core client.
 *
 * Requests are sent from the game client to the GGZ core client whenever
 * there is a need to change something about the game or to get more
 * information. The first group includes seat changes (standing up,
 * sitting down, booting players, adding bots and removing bots again).
 * The second group includes retrieving extended information about a
 * player or all players.
 * In addition, table chat can also happen through a request.
 *
 * When using \b KGGZSeatsDialog, everything except for chat can
 * be left to the dialog.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class Request
{
	public:
		/**
		 * The type of the request. Depending on this type,
		 * the object might be casted to a more specific
		 * class.
		 */
		enum Type
		{
			state,	/**< Change the state. */
			stand,	/**< Stand up from seat. */
			sit,	/**< Sit down again if possible. */
			boot,	/**< Boot a player from table. */
			bot,	/**< Let a bot join the game. */
			open,	/**< Open up a bot seat. */
			chat,	/**< Send a chat message. */
			info	/**< Request infos about a player. */
		};

		/**
		 * Creates a request of a specific type.
		 *
		 * The request will not be valid until all \ref data
		 * fields are properly filled out. It is therefore recommended
		 * to not use the request base class, but to use specific
		 * classes and their constructors which take care of this.
		 *
		 * @param type Type of the request
		 *
		 * @internal
		 */
		Request(Type type);

		/**
		 * Returns the type of a request.
		 *
		 * @return type of a request
		 */
		Type type() const;

		/**
		 * Data storage for all requests.
		 *
		 * Independent of the actual type of the request,
		 * all important data is stored in this map.
		 */
		QMap<QString, QString> data;

	private:
		Type m_type;
};

/**
 * @short State request to the GGZ core client.
 *
 * If the game should enter a new \ref Module::State, a request
 * of this type should be sent.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class StateRequest : public Request
{
	public:
		StateRequest(/*Module::State*/int state);
};

/**
 * @short Stand-up request to the GGZ core client.
 *
 * This request lets oneself change from player to spectator.
 * The seat occupied before will then be open.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class StandRequest : public Request
{
	public:
		StandRequest();
};

/**
 * @short Sit-down request to the GGZ core client.
 *
 * This request lets one become a player again if one is currently
 * a spectator.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class SitRequest : public Request
{
	public:
		SitRequest(int seat);
};

/**
 * @short Player-booting request to the GGZ core client.
 *
 * This request removes a player from the table.
 * The seat will then be open.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class BootRequest : public Request
{
	public:
		BootRequest(QString playername);
};

/**
 * @short Bot player addition request to the GGZ core client.
 *
 * This request puts a bot player into a specific seat,
 * which must be empty at the time of issueing this request.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class BotRequest : public Request
{
	public:
		BotRequest(int seat);
};

/**
 * @short Open seat request to the GGZ core client.
 *
 * To remove a bot player from the table, this request is
 * sent. The seat will then be open again.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class OpenRequest : public Request
{
	public:
		OpenRequest(int seat);
};

/**
 * @short Chat request to the GGZ core client.
 *
 * Sending a chat message to the other players at the same table,
 * that is, in the same game, is performed by sending a request
 * of this type.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class ChatRequest : public Request
{
	public:
		ChatRequest(QString message);
};

/**
 * @short Information request to the GGZ core client.
 *
 * Such requests retrieve additional player information.
 * If the \b seat parameter is included in the constructor,
 * only the information about a specific player is retrieved.
 * Otherwise, information about all players will be returned.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class InfoRequest : public Request
{
	public:
		InfoRequest(int seat);
		InfoRequest();
};

}

#endif

