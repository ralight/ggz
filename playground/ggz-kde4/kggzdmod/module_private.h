#ifndef KGGZDMOD_MODULE_PRIVATE_H
#define KGGZDMOD_MODULE_PRIVATE_H

#include <kggzdmod/module.h>
#include <kggzdmod/player.h>

#include <QList>

#include <ggzdmod.h>

namespace KGGZdMod
{

class Event;

class ModulePrivate : public QObject
{
	Q_OBJECT
	public:
		void connect();
		void disconnect();
		void sendRequest(Request request);
		void insertPlayer(Player::Type seattype, QString name, int seat);
		Player *findPlayer(Player::Type seattype, QString name);

		QString m_name;
		Module::State m_state;
		QList<Player*> m_players;
		QList<Player*> m_spectators;

		int m_playerseats;
		int m_spectatorseats;

	public slots:
		void slotGGZEvent();

	signals:
		void signalEvent(const KGGZdMod::Event& event);

	private:
		static void ggzdmod_handler(GGZdMod *mod, GGZdModEvent event, const void *data);
		void handler(GGZdMod *mod, GGZdModEvent event, const void *data);
};

}

#endif

