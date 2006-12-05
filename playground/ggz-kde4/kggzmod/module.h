#ifndef KGGZMOD_MODULE_H
#define KGGZMOD_MODULE_H

#include <kggzmod/request.h>
#include <kggzmod/event.h>

#include <QObject>
#include <QList>

namespace KGGZMod
{

class ModulePrivate;
class Player;

class Module : public QObject
{
	Q_OBJECT
	public:
		Module(QString name);
		~Module();

		enum State
		{
			created,
			connected,
			waiting,
			playing,
			done
		};

		void sendRequest(Request request);

		QList<Player*> players() const;
		QList<Player*> spectators() const;
		State state() const;
		//int fd() const;

		static bool isGGZ();

		Player *self() const;

	signals:
		void signalEvent(KGGZMod::Event event);
		void signalError();
		void signalNetwork(int fd);

	private:
		ModulePrivate *d;
};

}

#endif

