#ifndef CHESS_GGZ_H
#define CHESS_GGZ_H

#include <qsocket.h>
#include <qdatastream.h>

class GGZ : public QObject
{
	Q_OBJECT
	public:
		GGZ();
		~GGZ();
	
		void connect(const QString&);
		void disconnect();

		char getChar(void) { char value; Q_INT8 v; /*socket->readBlock(&value, sizeof(char));*/ *net >> v; value = v; return value; }
		void putChar(char value) { /*socket->writeBlock(&value, sizeof(char));*/ *net << (Q_INT8)value; }

		int getInt(void) { int value; /*socket->readBlock(&((char)value), sizeof(int));*/ *net >> value; return value; }
		void putInt(int value) { /*socket->writeBlock(&((char)value), sizeof(int));*/ *net << (Q_INT32)value;  }
	
		char *getString(void);
		QString getString(int maxlength);
		void putString(const QString&);
	
		// int readSocketDesc(void);

	public slots:
		void recvRawData();

	signals:
		void recvData();

	private:
		QSocket *socket;
		QDataStream *net;
};

#endif

