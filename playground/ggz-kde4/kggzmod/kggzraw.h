#ifndef KGGZGAMES_RAW_H
#define KGGZGAMES_RAW_H

#include <qobject.h>

class QAbstractSocket;
class QDataStream;

class KGGZRaw : public QObject
{
	Q_OBJECT
	public:
		KGGZRaw();
		~KGGZRaw();

		void setNetwork(int fd);
		bool hasMore();

		KGGZRaw& operator<<(qint32 i);
		KGGZRaw& operator<<(qint8 i);
		KGGZRaw& operator<<(const char *s);
		KGGZRaw& operator<<(QString s);

		KGGZRaw& operator>>(qint32 &i);
		KGGZRaw& operator>>(qint8 &i);
		KGGZRaw& operator>>(char *&s);
		KGGZRaw& operator>>(QString &s);

	signals:
		void signalError();

	private:
		bool ensureBytes(int bytes);

		QAbstractSocket *m_socket;
		QDataStream *m_net;
};

#endif

