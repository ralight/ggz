#include <qobject.h>
#include <qmap.h>
#include <qsocketnotifier.h>
#include <qsocketdevice.h>
#include <qdatastream.h>

namespace KGGZ
{

class Message
{
	public:
		Message(int type);

		int type();

		QMap<QString, QString> data;

	private:
		int m_type;
};

class Statistics
{
	public:
		Statistics();

		int wins;
		int losses;
		int ties;
		int forfeits;

		int rating;
		int ranking;
		int highscore;
};

class Player
{
	public:
		enum Type
		{
			open,
			bot,
			player,
			spectator
		};

		Player();
		Player(Type type);
		//Player& operator=(const Player& p);

		Type type();

		QString name;
		int seat;

		Statistics stats;

		QString hostname;
		QString photo;
		QString realname;

	private:
		int m_type;
};

class Module : public QObject
{
	Q_OBJECT
	public:
		Module(QString name);
		~Module();

		enum Event
		{
			launch,		/**< Game was launched. */
			server,		/**< Connect to server (-> host, port, player). */
			self,		/**< Your seat was assigned (-> player). */
			seat,		/**< Someone's seat changed (-> player). */
			chat,		/**< A chat message was received (-> player, message */
			stats		/**< Statistics have been received for all player. */
		};

		enum Request
		{
			reqstate,	/**< Change the state (-> state). */
			reqstand,	/**< Stand up from seat. */
			reqsit,		/**< Sit down again if possible. */
			reqboot,	/**< Boot a player from table (-> player). */
			reqbot,		/**< Let a bot join the game. */
			reqopen,	/**< Open up a reserved seat. */
			reqchat		/**< Send a chat message (-> message). */
		};

		enum State
		{
			created,
			connected,
			waiting,
			playing,
			done
		};

		void sendRequest(Message message);

		QValueList<Player> players();
		State state();
		int fd();

	signals:
		void signalEvent(Message message);
		void signalError();

	private:
		enum GGZEvents
		{
			msglaunch = 0,
			msgserver = 1,
			msgplayer = 2,
			msgseat = 3,
			msgspectatorseat = 4,
			msgchat = 5,
			msgstats = 6
		};

		void connect();
		void disconnect();
		void slotEvent();
		void insertPlayer(Player::Type seattype, QString name, int seat);

		QString m_name;
		int m_fd;
		State m_state;
		QValueList<Player> m_players;

		QSocketNotifier *m_notifier;
		QSocketDevice *m_dev;
		QDataStream *m_net;

		int m_playerseats;
		int m_spectatorseats;
};

}

