#include "kggzraw.h"

#include <kdebug.h>

#include <qabstractsocket.h>
#include <qdatastream.h>

/*
KGGZRaw provides a replacement for QDataStream when being used on an arbitrary
file descriptor, such as within the kggzmod library or for those games
which use a non-quantized binary protocol (i.e. blocking or in a thread).

This class is necessary for two reasons:
1) to ensure the QAbstractSocket is opened in unbuffered mode, so we can
   read ancillary data from the underlying fd as needed at any point in time
2) to ensure that QDataStream will block until enough bytes are available
   for reading
*/

/*
FIXME: one open issue is that when the destructor of QAbstractSocket is
called, the application will segfault!
*/

/*
FIXME: See qt-copy/doc/html/datastreamformat.html for compatibility with
easysock and other implementations
*/

KGGZRaw::KGGZRaw()
{
	m_socket = NULL;
	m_net = NULL;
}

KGGZRaw::~KGGZRaw()
{
kDebug() << "*destructor* net" << endl;
	delete m_net;
kDebug() << "*destructor* socket" << endl;
	delete m_socket;
kDebug() << "*destructor* done" << endl;
}

void KGGZRaw::setNetwork(int fd)
{
	// Ensure this method gets called only once
	if(m_socket)
	{
		kWarning() << "[raw] setNetwork called more than once" << endl;
		emit signalError();
		return;
	}

	// Create a datastream on an unbuffered TCP socket
	m_socket = new QAbstractSocket(QAbstractSocket::TcpSocket, this);
	m_socket->setSocketDescriptor(fd, QAbstractSocket::Connected, QIODevice::ReadWrite | QIODevice::Unbuffered);

	m_net = new QDataStream(m_socket);
}

bool KGGZRaw::ensureBytes(int bytes)
{
	// Ensure that kggzraw has been initialised properly
	if((!m_net) || (!m_socket))
	{
		kError() << "[raw] setNetwork not called yet" << endl;
		emit signalError();
		return false;
	}

	// Shortcut for outgoing data (operator<<)
	if(bytes == 0) return true;

	// Ensure that this number of bytes is available
	while(m_socket->bytesAvailable() < bytes)
	{
		m_socket->waitForReadyRead(-1);
		kDebug() << "[raw] bytesAvailable grows to: " << m_socket->bytesAvailable() << endl;
	}

	return true;
}

KGGZRaw& KGGZRaw::operator>>(qint32 &i)
{
	kDebug() << "[raw] bytesAvailable(32): " << m_socket->bytesAvailable() << endl;

	if(!ensureBytes(4)) return *this;
	*m_net >> i;

	kDebug() << "[raw] i is: " << i << endl;

	return *this;
}

KGGZRaw& KGGZRaw::operator>>(qint8 &i)
{
	kDebug() << "[raw] bytesAvailable(8): " << m_socket->bytesAvailable() << endl;

	if(!ensureBytes(1)) return *this;
	*m_net >> i;

	kDebug() << "[raw] i is: " << i << endl;

	return *this;
}

KGGZRaw& KGGZRaw::operator>>(char *&s)
{
	kDebug() << "[raw] bytesAvailable(s): " << m_socket->bytesAvailable() << endl;

	if(!ensureBytes(4)) return *this;
	// FIXME: ensure that 4 byte + N bytes are available - might need splitup
	*m_net >> s;

	kDebug() << "[raw] s is: " << s << endl;

	return *this;
}

KGGZRaw& KGGZRaw::operator>>(QString &s)
{
	kDebug() << "[raw] bytesAvailable(qstring): " << m_socket->bytesAvailable() << endl;

	if(!ensureBytes(4)) return *this;
	// FIXME: ensure that 4 byte + N bytes are available - might need splitup
	// FIXME: special stream format e.g. for null QString()s!
	*m_net >> s;

	kDebug() << "[raw] qstring is: " << s << endl;

	return *this;
}

KGGZRaw& KGGZRaw::operator<<(qint32 i)
{
	kDebug() << "[raw] out(32): " << i << endl;

	if(!ensureBytes(0)) return *this;
	*m_net << i;

	return *this;
}

KGGZRaw& KGGZRaw::operator<<(qint8 i)
{
	kDebug() << "[raw] out(8): " << i << endl;

	if(!ensureBytes(0)) return *this;
	*m_net << i;

	return *this;
}

KGGZRaw& KGGZRaw::operator<<(const char *s)
{
	kDebug() << "[raw] out(s): " << s << endl;

	if(!ensureBytes(0)) return *this;
	*m_net << s;

	return *this;
}

KGGZRaw& KGGZRaw::operator<<(QString s)
{
	kDebug() << "[raw] out(qstring): " << s << endl;

	if(!ensureBytes(0)) return *this;
	*m_net << s;

	return *this;
}

bool KGGZRaw::hasMore()
{
	return (m_socket->bytesAvailable() > 0);
}

#include "kggzraw.moc"
