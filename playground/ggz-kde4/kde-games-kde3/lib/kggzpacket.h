#ifndef KGGZGAMES_PACKET_H
#define KGGZGAMES_PACKET_H

#include <qobject.h>
#include <qdatastream.h>

class QSocketDevice;

class KGGZPacket : public QObject
{
	Q_OBJECT
	public:
		KGGZPacket();
		~KGGZPacket();

		QDataStream *inputstream();
		QDataStream *outputstream();

		void flush();

	public slots:
		void slotNetwork(int fd);

	signals:
 		void signalPacket();

	private:
		QDataStream *m_inputstream, *m_outputstream;
		QSocketDevice *m_socket;
		QByteArray m_input, m_output;
		int m_size;
};

#endif

