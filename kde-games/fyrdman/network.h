#ifndef FYRDMAN_NETWORK_H
#define FYRDMAN_NETWORK_H

#include <ggzmod.h>

#include <qobject.h>

class Network : public QObject
{
	Q_OBJECT
	public:
		Network();
		~Network();

		void connect();

		int fd();
		int cfd();

	public slots:
		void slotDispatch();

	signals:
		void signalData();

	private:
		int m_fd, m_cfd;

		static Network *me;
		static void handle(GGZMod *mod, GGZModEvent e, void *data);
		static GGZMod *mod;
};

#endif

