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
		void shutdown();

		int fd();
		int cfd();

		enum ServerToClient
		{
			msgseat = 0,
			msgplayers = 1,
			msgmove = 2,
			msggameover = 3,
			reqmove = 4,
			rspmove = 5,
			sndsync = 6,
			msgmaps = 7
		};

		enum ClientToServer
		{
			sndmove = 0,
			reqsync = 1,
			reqinit = 2,
			sndmap = 3
		};

		enum Errors
		{
			errstate = -1,
			errturn = -2,
			errbound = -3,
			errempty = -4,
			errfull = -5,
			errdist = -6,
			errmap = -7
		};

	public slots:
		void slotDispatch();

	signals:
		void signalData();

	private:
		int m_fd, m_cfd;

		static Network *me;
		static void handle(GGZMod *mod, GGZModEvent e,
				   const void *data);
		static GGZMod *mod;
};

#endif

