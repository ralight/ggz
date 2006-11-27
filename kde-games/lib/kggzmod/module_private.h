#ifndef KGGZMOD_MODULE_PRIVATE_H
#define KGGZMOD_MODULE_PRIVATE_H

#include <kggzmod/module.h>
#include <kggzmod/player.h>

#include <qsocketnotifier.h>
#include <qsocketdevice.h>
#include <qdatastream.h>

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

		QString m_name;
		int m_fd;
		Module::State m_state;
		QValueList<Player*> m_players;

		QSocketNotifier *m_notifier;
		QSocketDevice *m_dev;
		QDataStream *m_net;
		QSocketNotifier *m_gnotifier;

		int m_playerseats;
		int m_spectatorseats;

	public slots:
		void slotGGZEvent();

	signals:
		void signalEvent(KGGZMod::Event event);
		void signalError();
		void signalNetwork(int fd);
};

}

#endif

