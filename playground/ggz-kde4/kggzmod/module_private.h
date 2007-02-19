#ifndef KGGZMOD_MODULE_PRIVATE_H
#define KGGZMOD_MODULE_PRIVATE_H

#include <kggzmod/module.h>
#include <kggzmod/player.h>

#include <QSocketNotifier>
#include <QList>

class KGGZRaw;

namespace KGGZMod
{

class ModulePrivate : public QObject
{
	Q_OBJECT
	public:
		enum GGZEvents
		{
			msglaunch = 0,
			msgserver = 1,
			msgserverfd = 2,
			msgplayer = 3,
			msgseat = 4,
			msgspectatorseat = 5,
			msgchat = 6,
			msgstats = 7,
			msginfo = 8
		};

		void connect();
		void disconnect();
		void sendRequest(Request request);
		void insertPlayer(Player::Type seattype, QString name, int seat);
		Player *findPlayer(Player::Type seattype, QString name);
		Player *self() const;

		QString m_name;
		int m_fd;
		Module::State m_state;
		QList<Player*> m_players;
		QList<Player*> m_spectators;

		QSocketNotifier *m_notifier;
		QSocketNotifier *m_gnotifier;
		KGGZRaw *m_net;

		int m_playerseats;
		int m_spectatorseats;

		int m_myseat;
		bool m_myspectator;

	public slots:
		void slotGGZEvent();

	signals:
		void signalEvent(const KGGZMod::Event& event);
		void signalError();
		void signalNetwork(int fd);
};

}

#endif

