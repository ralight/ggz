#ifndef GGZ_C_H
#define GGZ_C_H

#include <qsocket.h>

class GGZ : public QObject
{
	Q_OBJECT
	public:
		GGZ();
		~GGZ();
	
		void connect(QString&);
		void disconnect();

		char getChar(void) { char value; socket->readBlock(&value, sizeof(char)); return value; }
		void putChar(char value) { socket->writeBlock(&value, sizeof(char)); }

		int getInt(void) { int value; socket->readBlock(&((char)value), sizeof(int)); return value; }
		void putInt(int value) { socket->writeBlock(&((char)value), sizeof(int)); }
	
		QString getString(void);
		QString getString(int maxlength);
		void putString(const QString&);
	
		// int readSocketDesc(void);

	signals:
		void recvData();

	private:
		QSocket *socket;
};

#endif

