#ifndef KGGZDMOD_EVENT_H
#define KGGZDMOD_EVENT_H

#include <QMap>
#include <QString>

#include <kggzdmod/module.h>

// FIXME: use Player or separate classes?
#define PlayerSeat Player
#define SpectatorSeat Player

namespace KGGZdMod
{

class EventPrivate;
class Player;

/**
 * @short Events from the GGZ server.
 *
 * The GGZ server sends signals to the game server whenever important
 * changes happen, for example when players join or an error occurs.
 * The signal is sent through \ref Module::signalEvent.
 *
 * Depending on the type of the event, the more general \ref Event object
 * can be casted to specific objects of type \ref StateEvent, \ref
 * PlayerSeatEvent, \ref SpectatorSeatEvent, \ref PlayerDataEvent, \ref
 * SpectatorDataEvent and \ref ErrorEvent.
 * However, this is not strictly needed, as all data
 * is available from the public \ref data hashtable, should a game
 * wish to use this instead.
 *
 * In order to implement a fully working GGZ game server, all of the events
 * should be handled. (This is different from game client events in kggzmod!)
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class Event
{
	friend class ModulePrivate;
	friend class StateEvent;
	friend class PlayerDataEvent;
	friend class SpectatorDataEvent;
	friend class PlayerSeatEvent;
	friend class SpectatorSeatEvent;
	friend class ErrorEvent;

	public:
		/**
		 * The type of the event. Depending on this type,
		 * the object might be casted to a more specific
		 * class.
		 */
		enum Type
		{
			state,
			playerseat,
			spectatorseat,
			playerdata,
			spectatordata,
			error
		};

		/**
		 * Creates an event of a specific type.
		 *
		 * The event will not be valid until all \ref data fields
		 * are properly filled out. Therefore, only the \ref Module
		 * is supposed to create event objects.
		 *
		 * @param type Type of the event
		 *
		 * @internal
		 */
		//Event(Type type);

		/**
		 * Returns the type of an event.
		 *
		 * @return type of an event
		 */
		Type type() const;

		/**
		 * Data storage for all events.
		 *
		 * Independent of the actual type of the event,
		 * all important data is stored in this map.
		 */
		//QMap<QString, QString> data;

	private:
		Event(Type type);
		EventPrivate *d;
		Type m_type;
		void init(EventPrivate *x);
};

/**
 * @short State change event from the GGZ server.
 *
 * The game server will be launched in \ref created state, and move
 * to \ref waiting as soon as it has received all the essential
 * information from the GGZ server. Afterwards, the game server itself
 * will control the state, toggling between \ref waiting and \ref
 * playing, and eventually changing the state to \done.
 *
 * Events of this type notify about state changes and report the
 * old state and the new state.
 *
 * Refer to the \ref Event documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class StateEvent : public Event
{
	public:
		StateEvent(const Event& event);
		Module::State state() const;
		Module::State oldstate() const;
};

/**
 * @short A seat was changed.
 *
 * This event happens whenever the occupation of a player seat changes.
 * Many different transitions will lead to this event, such as players
 * leaving the game (\ref standup) or joining the game (\ref sitdown),
 * bot changes and reservation status changes.
 *
 * The old and the new seat are reported and should be used for a
 * detailed comparison, which might include name changes. The basic
 * transition is reported through \ref change as a convenience method.
 *
 * Refer to the \ref Event documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class PlayerSeatEvent : public Event
{
	public:
		PlayerSeatEvent(const Event& event);
		PlayerSeat seat() const;
		PlayerSeat oldseat() const;
		enum PlayerSeatChange
		{
			sitdown,
			standup,
			botsitdown,
			botstandup,
			botswap,
			reservation,
			dropreservation
		};
		PlayerSeatChange change() const;
};

/**
 * @short A player sent data to the game server.
 *
 * If a seat is occupied with a real player (human or external AI),
 * meaning that the seat has a valid file descriptor, it is natural that
 * data can arrive on the descriptor. This event informs about the
 * affected seat.
 *
 * Refer to the \ref Event documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class PlayerDataEvent : public Event
{
	public:
		PlayerDataEvent(const Event& event);
		PlayerSeat seat() const;
};

/**
 * @short A spectator seat was changed.
 *
 * This event happens whenever the occupation of a spectator seat changes.
 * There are only two possible transitions behind this: spectators
 * joining the game (\ref sitdown) or leaving the game (\ref standup).
 *
 * The old and the new seat are reported and should be used for a
 * detailed comparison, which might include name changes. The basic
 * transition is reported through \ref change as a convenience method.
 *
 * Refer to the \ref Event documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class SpectatorSeatEvent : public Event
{
	public:
		SpectatorSeatEvent(const Event& event);
		SpectatorSeat seat() const;
		SpectatorSeat oldseat() const;
		enum SpectatorSeatChange
		{
			sitdown,
			standup
		};
		SpectatorSeatChange change() const;
};

/**
 * @short A spectator sent data to the game server.
 *
 * While spectators cannot influence a game, they might want to change
 * their viewport or request other information about the game from the
 * game server. This event informs about the affected spectator.
 *
 * Refer to the \ref Event documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class SpectatorDataEvent : public Event
{
	public:
		SpectatorDataEvent(const Event& event);
		SpectatorSeat seat() const;
};

/**
 * @short Something went wrong with the game.
 *
 * Whenever something bad happens with the game server, this
 * event is called. The game server should shut down gracefully
 * since it will almost always be impossible to continue the game
 * in such situations.
 *
 * Refer to the \ref Event documentation for everything else.
 *
 * @author Josef Spillner (josef@ggzgamingzone.org)
 */
class ErrorEvent : public Event
{
	public:
		ErrorEvent(const Event& event);
		QString message() const;
};

}

#endif

