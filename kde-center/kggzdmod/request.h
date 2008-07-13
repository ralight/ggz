#ifndef KGGZDMOD_REQUEST_H
#define KGGZDMOD_REQUEST_H

#include <QMap>
#include <QString>

namespace KGGZdMod
{

/**
 * @short Requests to the GGZ server.
 *
 * Requests are sent from the game server to the GGZ server whenever
 * there is a need to change something about the game or to get more
 * information.
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
			state,		/**< Change the state. */
			log,		/**< Log a message. */
			result,		/**< Report the game results. */
			savegame,	/**< Report a savegame. */
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
 * @short State request to the GGZ server.
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
 * @short Message logging request to the GGZ server.
 *
 * Log messages should not simply be written to the standard
 * output. Instead, the GGZ server receives them and can treat
 * them according to the GGZ logging policies.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class LogRequest : public Request
{
	public:
		LogRequest(QString message);
};

/**
 * @short Reports the result of a game.
 *
 * This request is usually sent at the end of a game to put the game
 * results into the statistics database.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class ResultRequest : public Request
{
	public:
		ResultRequest();
};

/**
 * @short Reports the location of a savegame.
 *
 * Since savegames are often continuous logs which can be used to
 * replay a game or continue an interrupted game, requests of this type
 * should be sent whenever possible, but at least when the savegame
 * file is created and when it is finished.
 *
 * Refer to the \ref Request documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class SavegameRequest : public Request
{
	public:
		SavegameRequest(QString filename);
};

}

#endif

