#include "kggzpacket.h"

#include <kdebug.h>

#include <qsocketdevice.h>

KGGZPacket::KGGZPacket()
{
	m_socket = NULL;

	m_inputstream = new QDataStream(m_input, IO_ReadOnly);
	m_outputstream = new QDataStream(m_output, IO_WriteOnly);
}

KGGZPacket::~KGGZPacket()
{
	if(m_socket)
	{
		flush();
		delete m_socket;
	}
}

QDataStream *KGGZPacket::inputstream()
{
	return m_inputstream;
}

QDataStream *KGGZPacket::outputstream()
{
	return m_outputstream;
}

void KGGZPacket::flush()
{
	QByteArray packsize;
	QDataStream packsizestream(packsize, IO_WriteOnly);
	packsizestream << (Q_INT16)(m_output.size() + 2);

	kdDebug() << "<kggzpacket> flush; packsize = " << m_output.size() << endl;

	m_socket->writeBlock(packsize.data(), packsize.size());
	m_socket->writeBlock(m_output.data(), m_output.size());
	m_output.truncate(0);
}

void KGGZPacket::slotNetwork(int fd)
{
	Q_LONG avail;
	Q_ULONG len;
	QByteArray packsize;
	QDataStream packsizestream(packsize, IO_ReadOnly);
	Q_INT16 size;

	if(!m_socket)
	{
		kdDebug() << "<kggzpacket> init socket device" << endl;
		m_socket = new QSocketDevice(fd, QSocketDevice::Stream);
	}

	if(m_input.size() == 0)
	{
		if(m_socket->bytesAvailable() < 2) return;
		packsize.resize(2);
		avail = m_socket->readBlock(packsize.data(), 2);
		if(avail == -1)
		{
			// Error!
		}
		packsizestream >> size;
		m_size = (int)size - 2;
		m_input.resize(m_size);
		kdDebug() << "<kggzpacket> input init; packsize = 2 + " << m_size << endl;
	}

	len = m_socket->bytesAvailable();
	if(len > m_size - m_input.size()) len = m_size - m_input.size();
	avail = m_socket->readBlock(m_input.data() + m_input.size(), len);
	if(avail == -1)
	{
		// Error!
	}
	kdDebug() << "<kggzpacket> input; read up to " << m_input.size() << endl;

	if(m_input.size() == (Q_ULONG)m_size)
	{
		kdDebug() << "<kggzpacket> input done for packet; fire signal!" << endl;
		emit signalPacket();
		m_input.truncate(0);
	}
}

