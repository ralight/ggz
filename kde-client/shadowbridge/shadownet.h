#ifndef SHADOW_NET_H
#define SHADOW_NET_H

#include <qobject.h>
#include <qlist.h>

class ShadowNet : public QObject
{
	Q_OBJECT
	public:
		ShadowNet();
		~ShadowNet();
		void addClient(const char *name, const char *commandline);
		void start();
	public slots:
		void slotRead(int sock);
		void slotWrite(int sock);
	signals:
		void signalIncoming(const char *data);
		void signalOutgoing(const char *data);
	private:
		QList<char> list;
};

#endif

